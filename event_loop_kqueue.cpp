/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_loop.hpp"

#include "channel_base.hpp"
#include "logger.hpp"
#include "serv_exception.hpp"
#include "serv_types.hpp"
#include "socket_utils.hpp"

#include "pthread_wrapper.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <unistd.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

typedef ft::serv::dynamic_array<struct ::kevent>::type event_list;

static const event_list::size_type MAX_EVENTS = 4096;
static const ft::serv::ident_t WAKE_UP_IDENT = 0;

ft::serv::event_loop::event_loop()
    : boss_fd(::kqueue())
{
    if (this->boss_fd < 0)
    {
        throw syscall_failed();
    }

    struct ::kevent change[1];
    EV_SET(&change[0], WAKE_UP_IDENT, EVFILT_USER, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_TRIGGER | NOTE_FFNOP, 0, null);
    if (::kevent(this->boss_fd, beginof(change), countof(change), null, 0, null) < 0)
    {
        const syscall_failed e;
        close_socket(this->boss_fd);
        throw e;
    }

    error_t error_mutex = ::pthread_mutex_init(&this->lock, null);
    if (error_mutex != 0)
    {
        const syscall_failed e(error_mutex);
        close_socket(this->boss_fd);
        throw e;
    }
}

ft::serv::event_loop::~event_loop()
{
    ::pthread_mutex_destroy(&this->lock);
    close_socket(this->boss_fd);
}

void ft::serv::event_loop::add_channel(const ident_t ident, const ft::shared_ptr<channel_base>& channel)
{
    // TODO: assert(this->is_in_event_loop());

    bool success = this->channels.insert(std::make_pair(ident, channel)).second;

    if (success)
    {
        event_list& changes = *static_cast<event_list*>(this->boss_list);
        struct ::kevent change[2];
        EV_SET(&change[0], ident, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, null);
        EV_SET(&change[1], ident, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, null);
        changes.insert(changes.end(), beginof(change), &change[countof(change)]);
    }
    else
    {
        // TODO: reject
    }
}

void ft::serv::event_loop::remove_channel(const ident_t ident)
{
    // TODO: assert(this->is_in_event_loop());

    bool success = this->channels.erase(ident) != 0;

    if (success)
    {
        event_list& changes = *static_cast<event_list*>(this->boss_list);
        struct ::kevent change[2];
        EV_SET(&change[0], ident, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, null);
        EV_SET(&change[1], ident, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, null);
        changes.insert(changes.end(), beginof(change), &change[countof(change)]);
    }
    else
    {
        // ignore
    }
}

void ft::serv::event_loop::watch_writability(const ident_t ident, const bool enable)
{
    // TODO: assert(this->is_in_event_loop());

    event_list& changes = *static_cast<event_list*>(this->boss_list);
    struct ::kevent change[1];
    EV_SET(&change[0], ident, EVFILT_WRITE, enable ? EV_ADD | EV_ENABLE | EV_CLEAR : EV_DELETE | EV_DISABLE, 0, 0, null);
    changes.insert(changes.end(), beginof(change), &change[countof(change)]);
}

void ft::serv::event_loop::offer_task(const ft::shared_ptr<task_base> task)
{
    ft::pthread_mutex_guard(this->lock);
    try
    {
        this->tasks.push_back(task);
    }
    catch (const std::exception& e)
    {
        // TODO: reject
    }
}

void ft::serv::event_loop::loop()
{
    event_list events;
    event_list changes;

    events.resize(MAX_EVENTS);
    changes.reserve(MAX_EVENTS);

    this->boss_list = &changes;

    for (;;)
    {
        ::time_t timeout_sec;
        {
            pthread_mutex_guard(this->lock);
            // if has task then polling
            timeout_sec = this->tasks.empty() ? 0 : 1;
        }
        const struct ::timespec timeout = {timeout_sec, 0};
        const int n = ::kevent(this->boss_fd, changes.data(), changes.size(), events.data(), events.size(), &timeout);
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
            events.clear();
        }
        this->execute_tasks();
    }
}

void ft::serv::event_loop::wake_up()
{
    // TODO: assert(this->is_in_event_loop());

    struct ::kevent change[1];
    EV_SET(&change[0], WAKE_UP_IDENT, EVFILT_USER, 0, NOTE_TRIGGER | NOTE_FFNOP, 0, null);
    int n = ::kevent(this->boss_fd, beginof(change), countof(change), null, 0, null);
    if (n < 0)
    {
        const syscall_failed e;
        if (e.error() != EINTR)
        {
            throw e;
        }
    }
}

void ft::serv::event_loop::process_events(void* list, int n) throw()
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
        const ft::shared_ptr<channel_base>& channel = it_channel->second;

        if (evi.filter == EVFILT_WRITE)
        {
            channel->_internal_trigger_write();
        }
        else if (evi.filter == EVFILT_READ)
        {
            // TODO: use `evi.data`
            channel->_internal_trigger_read();
        }

        if (evi.flags & EV_EOF)
        {
            // TODO: _internal_trigger_read_eof, RDHUP?
        }
    }
}

void ft::serv::event_loop::execute_tasks() throw()
{
    task_list snapshot;
    {
        ft::pthread_mutex_guard(this->lock);
        snapshot = this->tasks;
        this->tasks.clear();
    }

    for (task_list::iterator it = snapshot.begin(); it != snapshot.end(); ++it)
    {
        ft::shared_ptr<task_base> task = *it;
        task->run();
    }
}
