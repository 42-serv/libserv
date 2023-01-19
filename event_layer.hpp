/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

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
            event_layer* next;
            event_layer* prev;
            ft::shared_ptr<event_handler_base> handler;

        public:
            event_layer(event_channel_base& channel, event_layer* next, event_layer* prev, const ft::shared_ptr<event_handler_base>& handler);
            virtual ~event_layer();

            virtual void notify_active();
            virtual void notify_read(void*);
            virtual void notify_error(const std::exception&);
            virtual void notify_inactive();

            virtual void post_write(void*);
            virtual void post_flush();
            virtual void post_disconnect();

        private:
            event_layer(const event_layer&);
            event_layer& operator=(const event_layer&);
        };
    }
}
