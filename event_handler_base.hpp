/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

namespace ft
{
    namespace serv
    {
        class event_layer;

        class event_handler_base
        {
        public:
            virtual ~event_handler_base() {}

            virtual void on_active(event_layer&) = 0;
            virtual void on_read(event_layer&) = 0;
            virtual void on_write(event_layer&) = 0;
            virtual void on_error(event_layer&) = 0;
            virtual void on_inactive(event_layer&) = 0;

        private:
            event_handler_base(const event_handler_base&);
            event_handler_base& operator=(const event_handler_base&);
        };
    }
}
