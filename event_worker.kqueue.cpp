/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_worker.hpp"

#include "event_channel_base.hpp"
#include "logger.hpp"
#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/condition_variable.hpp>
#include <thread/mutex.hpp>
#include <thread/thread.hpp>

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <exception>
#include <map>
#include <string>
#include <vector>

namespace ft
{
    namespace serv
    {
        typedef dynamic_array<struct ::kevent>::type event_list;

        static const event_list::size_type MAX_EVENTS = FT_SERV_MAX_EVENT_SIZE;
    }
}

ft::serv::event_worker::event_worker()
    : lock(),
      cond(),
      active(),
      loop_ident(::kqueue()),
      event_ident(0),
      loop_list(),
      channels(),
      tasks(),
      task_closed(),
      working_thread()
{
    if (this->loop_ident < 0)
    {
        throw syscall_failed();
    }

    struct ::kevent change[1];
    EV_SET(&change[0], this->event_ident, EVFILT_USER, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_TRIGGER | NOTE_FFNOP, 0, null);
    if (::kevent(this->loop_ident, beginof(change), countof(change), null, 0, null) < 0)
    {
        const syscall_failed e;
        ::close(this->loop_ident);
        throw e;
    }
}

ft::serv::event_worker::~event_worker()
{
    ::close(this->loop_ident);
}

void ft::serv::event_worker::add_channel(const ft::shared_ptr<event_channel_base>& channel)
{
    assert(this->is_in_event_loop());

    const ident_t ident = channel->get_ident();

    const bool success = this->channels.insert(std::make_pair(ident, channel)).second;
    assert(success);

    this->watch_ability(*channel);
}

void ft::serv::event_worker::remove_channel(const ident_t ident)
{
    assert(this->is_in_event_loop());

    channel_dictionary::/*const_*/ iterator it = this->channels.find(ident);

    if (it != this->channels.end())
    {
        const ft::shared_ptr<event_channel_base>& channel = it->second;
        this->watch_ability(*channel);
        this->channels.erase(it);
    }
    else
    {
        // ignore
    }
}

void ft::serv::event_worker::watch_ability(event_channel_base& channel)
{
    assert(this->is_in_event_loop());

    const int flags_add = EV_ADD | EV_ENABLE | EV_CLEAR;
    const int flags_del = EV_DELETE | EV_DISABLE;

    const ident_t ident = channel.get_ident();
    event_list& changes = *static_cast<event_list*>(this->loop_list);
    struct ::kevent change[2];
    event_list::size_type count = 0;
    bool interested[2];
    bool changed[2];
    channel.load_interested(interested, changed);
    if (changed[0])
    {
        EV_SET(&change[count++], ident, EVFILT_READ, interested[0] ? flags_add : flags_del, 0, 0, null);
    }
    if (changed[1])
    {
        EV_SET(&change[count++], ident, EVFILT_WRITE, interested[1] ? flags_add : flags_del, 0, 0, null);
    }
    changes.insert(changes.end(), beginof(change), &change[count]);
    channel.store_interested();
    this->wake_up();
}

void ft::serv::event_worker::loop()
{
    this->working_thread = ft::thread::self();
    synchronized(this->lock)
    {
        this->active = true;
    }
    this->cond.notify_all();

    event_list events;
    event_list changes;

    events.resize(MAX_EVENTS);
    changes.reserve(MAX_EVENTS);

    this->loop_list = &changes;

    for (;;)
    {
        ::time_t timeout_sec;
        synchronized(this->lock)
        {
            // if has task then polling
            timeout_sec = this->tasks.empty() ? 1 : 0;
        }
        const struct ::timespec timeout = {timeout_sec, 0};
        const int n = ::kevent(this->loop_ident, changes.data(), changes.size(), events.data(), events.size(), &timeout);
        if (n < 0)
        {
            const syscall_failed e;
            if (e.error() == EINTR)
            {
                // When kevent() call fails with EINTR error, all changes in the changelist have been applied.
                // https://www.freebsd.org/cgi/man.cgi?query=kqueue&sektion=2
                changes.clear();
            }
            else
            {
                logger::warn("Error on kevent");
                ::sleep(1);
            }
            continue;
        }

        changes.clear();
        if (n != 0)
        {
            this->process_events(&events, n);
        }

        if (!this->execute_tasks())
        {
            break;
        }

        if (static_cast<event_list::size_type>(n) == events.size())
        {
            // double
            events.resize(n << 1);
        }
    }
}

void ft::serv::event_worker::wake_up() throw()
{
    if (!this->is_in_event_loop())
    {
        struct ::kevent change[1];
        EV_SET(&change[0], this->event_ident, EVFILT_USER, 0, NOTE_TRIGGER | NOTE_FFNOP, 0, null);
        if (::kevent(this->loop_ident, beginof(change), countof(change), null, 0, null) < 0)
        {
            const syscall_failed e;
            if (e.error() != EINTR)
            {
                // ignore errors
            }
        }
    }
}

void ft::serv::event_worker::process_events(void* list, int n) throw()
{
    event_list& events = *static_cast<event_list*>(list);

    for (int i = 0; i < n; i++)
    {
        const struct ::kevent& evi = events[i];
        // evi.udata: event_invoker

        if (evi.filter == EVFILT_USER)
        {
            // wakeup?
            continue;
        }

        if (evi.flags & EV_ERROR)
        {
            // double delete? (close after delete)
            continue;
        }

        const ident_t ident = evi.ident;
        const channel_dictionary::const_iterator it_channel = this->channels.find(ident);
        if (it_channel == this->channels.end())
        {
            // no channel!!
            continue;
        }
        const ft::shared_ptr<event_channel_base>& channel = it_channel->second;

        if (evi.filter == EVFILT_WRITE)
        {
            channel->trigger_write();
        }
        else if (evi.filter == EVFILT_READ)
        {
            // TODO: use `evi.data`
            channel->trigger_read();
        }

        if (evi.flags & EV_EOF)
        {
            // TODO: trigger_read_eof, RDHUP?
        }
    }
}
