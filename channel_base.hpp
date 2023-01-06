/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

namespace ft
{
    namespace serv
    {
        class channel_base
        {
        public:
            virtual ~channel_base() {}

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

            void _internal_trigger_read() throw();
            void _internal_trigger_write() throw();

        private:
            channel_base(const channel_base&);
            channel_base& operator=(const channel_base&);
        };
    }
}
