/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class event_layer
        {
        private:
            event_layer* next;
            event_layer* prev;
            ft::shared_ptr<event_handler_base> handler;

        public:
            virtual void notify_active() = 0;
            virtual void notify_read() = 0;
            virtual void notify_write() = 0;
            virtual void notify_error() = 0;
            virtual void notify_inactive() = 0;

            virtual void post_bind() = 0;
            virtual void post_connect() = 0;
            virtual void post_read() = 0;
            virtual void post_write() = 0;
            virtual void post_flush() = 0;
            virtual void post_disconnect() = 0;

        private:
            event_layer(const event_layer&);
            event_layer& operator=(const event_layer&);
        };
    }
}
