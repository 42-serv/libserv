/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_worker.hpp"

#include "event_channel_base.hpp"
#include "logger.hpp"
#include "serv_exception.hpp"
#include "serv_types.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/condition_variable.hpp>
#include <thread/lock_guard.hpp>
#include <thread/mutex.hpp>
#include <thread/thread.hpp>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <exception>
#include <map>
#include <string>
#include <vector>

namespace ft
{
    namespace serv
    {
        typedef dynamic_array<struct ::epoll_event>::type event_list;

        static const event_list::size_type MAX_EVENTS = 4096;

        static void _epoll_operation(ident_t epoll_fd, int epoll_operation, event_channel_base& channel)
        {
            int flags = 0;
            if (channel.readability_interested)
            {
                flags |= EPOLLIN;
            }
            if (channel.writability_interested)
            {
                flags |= EPOLLOUT;
            }
            const ident_t ident = channel.get_ident();
            struct ::epoll_event change;
            change.events = flags;
            change.data.fd = ident;
            if (::epoll_ctl(epoll_fd, epoll_operation, ident, &change) < 0)
            {
                throw syscall_failed();
            }
            channel.readability_enabled = channel.readability_interested;
            channel.writability_enabled = channel.writability_interested;
        }
    }
}

ft::serv::event_worker::event_worker()
    : lock(),
      cond(),
      boss_ident(::epoll_create1(EPOLL_CLOEXEC)),
      event_ident(0),
      boss_list(),
      channels(),
      tasks(),
      loop_thread()
{
    try
    {
        if (this->boss_ident < 0)
        {
            throw syscall_failed();
        }

        this->event_ident = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (this->event_ident < 0)
        {
            throw syscall_failed();
        }

        struct ::epoll_event change;
        change.events = EPOLLIN;
        change.data.fd = this->event_ident;
        if (::epoll_ctl(this->boss_ident, EPOLL_CTL_ADD, this->event_ident, &change) < 0)
        {
            throw syscall_failed();
        }
    }
    catch (const syscall_failed& e)
    {
        if (!(this->boss_ident < 0))
        {
            socket_utils::close_socket(this->boss_ident);
        }
        if (!(this->event_ident < 0))
        {
            socket_utils::close_socket(this->event_ident);
        }
        throw;
    }
}

ft::serv::event_worker::~event_worker()
{
    socket_utils::close_socket(this->boss_ident);
    socket_utils::close_socket(this->event_ident);
}

void ft::serv::event_worker::add_channel(const ft::shared_ptr<event_channel_base>& channel)
{
    assert(this->is_in_event_loop());

    const ident_t ident = channel->get_ident();

    const bool success = this->channels.insert(std::make_pair(ident, channel)).second;
    assert(success);

    channel->readability_interested = true;
    channel->writability_interested = true;
    _epoll_operation(this->boss_ident, EPOLL_CTL_ADD, *channel);
}

void ft::serv::event_worker::remove_channel(const ident_t ident)
{
    assert(this->is_in_event_loop());

    channel_dictionary::/*const_*/ iterator it = this->channels.find(ident);

    if (it != this->channels.end())
    {
        const ft::shared_ptr<event_channel_base>& channel = it->second;
        channel->readability_interested = false;
        channel->writability_interested = false;
        _epoll_operation(this->boss_ident, EPOLL_CTL_DEL, *channel);
        this->channels.erase(it->first);
    }
    else
    {
        // ignore
    }
}

void ft::serv::event_worker::watch_ability(event_channel_base& channel)
{
    assert(this->is_in_event_loop());

    _epoll_operation(this->boss_ident, EPOLL_CTL_MOD, channel);
}

void ft::serv::event_worker::loop()
{
    this->loop_thread = ft::thread::self();
    this->cond.notify_all();

    event_list events;

    events.resize(MAX_EVENTS);

    this->boss_list = null;

    for (;;)
    {
        ::time_t timeout_millis;
        {
            const ft::lock_guard<ft::mutex> lock(this->lock);
            // if has task then polling
            timeout_millis = this->tasks.empty() ? 0 : 1000;
        }
        const int n = ::epoll_wait(this->boss_ident, events.data(), events.size(), timeout_millis);
        if (n < 0)
        {
            const syscall_failed e;
            if (e.error() == EINTR)
            {
                // ignore
            }
            else
            {
                logger::warn("Error on epoll_wait");
                ::sleep(1);
            }
            continue;
        }

        if (n != 0)
        {
            this->process_events(&events, n);
        }
        this->execute_tasks();

        if (static_cast<event_list::size_type>(n) == events.size())
        {
            // double
            events.resize(n << 1);
        }
    }
}

void ft::serv::event_worker::wake_up()
{
    if (!this->is_in_event_loop())
    {
        ::eventfd_t value = 1;
        const int r = ::eventfd_write(this->event_ident, value);
        // ignore errors
        static_cast<void>(r);
    }
}

void ft::serv::event_worker::process_events(void* list, int n) throw()
{
    event_list& events = *static_cast<event_list*>(list);

    for (int i = 0; i < n; i++)
    {
        const struct ::epoll_event& evi = events[i];

        const ident_t ident = evi.data.fd;
        if (ident == this->event_ident)
        {
            // wakeup?
            ::eventfd_t value;
            const int r = ::eventfd_read(evi.data.fd, &value);
            // ignore errors
            static_cast<void>(r);
            continue;
        }

        const channel_dictionary::const_iterator it_channel = this->channels.find(ident);
        if (it_channel == this->channels.end())
        {
            // no channel!!
            const int r = ::epoll_ctl(this->boss_ident, EPOLL_CTL_DEL, ident, null);
            // ignore errors
            static_cast<void>(r);
            continue;
        }
        const ft::shared_ptr<event_channel_base>& channel = it_channel->second;

        if (evi.events & (EPOLLOUT | EPOLLERR))
        {
            channel->trigger_write();
        }

        if (evi.events & (EPOLLIN | EPOLLERR))
        {
            // TODO: use `evi.data`
            channel->trigger_read();
        }

        if (evi.events & EPOLLRDHUP)
        {
            // TODO: trigger_read_eof, RDHUP?
        }
    }
}
