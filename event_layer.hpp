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
            event_layer() {}
            virtual ~event_layer() {}

            virtual void notify_active() {}
            virtual void notify_read() {}
            virtual void notify_write() {}
            virtual void notify_error() {}
            virtual void notify_inactive() {}

            virtual void post_bind() {}
            virtual void post_connect() {}
            virtual void post_read() {}
            virtual void post_write() {}
            virtual void post_flush() {}
            virtual void post_disconnect() {}

        private:
            event_layer(const event_layer&);
            event_layer& operator=(const event_layer&);
        };
    }
}
