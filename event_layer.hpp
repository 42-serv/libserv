/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <smart_ptr/weak_ptr.hpp>

#include <exception>

namespace ft
{
    namespace serv
    {
        class event_channel_base;

        class event_layer
        {
        private:
            event_channel_base& channel;
            ft::shared_ptr<event_handler_base> handler;
            ft::shared_ptr<event_layer> next;
            ft::weak_ptr<event_layer> prev;

        public:
            event_layer(event_channel_base& channel, const ft::shared_ptr<event_handler_base>& handler);
            ~event_layer();

        public:
            void set_next(const ft::shared_ptr<event_layer>& next);
            void set_prev(const ft::shared_ptr<event_layer>& prev);

        private:
            void on_active();
            void on_read(ft::shared_ptr<void>);
            void on_read_complete();
            void on_error(ft::shared_ptr<const std::exception>);
            void on_inactive();

            void do_register(ft::shared_ptr<void>);
            void do_write(ft::shared_ptr<const void>);
            void do_flush();
            void do_disconnect();
            void do_deregister();

        private:
            void invoke_on_active();
            void invoke_on_read(ft::shared_ptr<void>);
            void invoke_on_read_complete();
            void invoke_on_error(ft::shared_ptr<const std::exception>);
            void invoke_on_inactive();

            void invoke_do_register(ft::shared_ptr<void>);
            void invoke_do_write(ft::shared_ptr<const void>);
            void invoke_do_flush();
            void invoke_do_disconnect();
            void invoke_do_deregister();

        public:
            void notify_active();
            void notify_read(ft::shared_ptr<void>);
            void notify_read_complete();
            void notify_error(ft::shared_ptr<const std::exception>);
            void notify_inactive();

            void post_register(ft::shared_ptr<void>);
            void post_write(ft::shared_ptr<const void>);
            void post_flush();
            void post_disconnect();
            void post_deregister();

        private:
            event_layer(const event_layer&);
            event_layer& operator=(const event_layer&);
        };
    }
}
