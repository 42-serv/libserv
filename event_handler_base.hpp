/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include <exception>

namespace ft
{
    namespace serv
    {
        class event_layer;

        class event_handler_base
        {
        public:
            event_handler_base();
            virtual ~event_handler_base();

            virtual void on_active(event_layer& next);
            virtual void on_read(event_layer& next, void* arg);
            virtual void on_error(event_layer& next, const std::exception& e);
            virtual void on_inactive(event_layer& next);

            virtual void on_write(event_layer& prev, void* arg);
            virtual void on_flush(event_layer& prev);
            virtual void on_disconnect(event_layer& prev);

        private:
            event_handler_base(const event_handler_base&);
            event_handler_base& operator=(const event_handler_base&);
        };
    }
}
