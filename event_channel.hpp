/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "byte_buffer.hpp"
#include "serv_types.hpp"

namespace ft
{
    namespace serv
    {
        class event_channel
        {
        private:
            byte_buffer recv_buf;
            byte_buffer send_buf;
            // pipeline_head
            // pipeline_tail
            // local_address
            // remote_address
            // closed

        public:
            virtual ~event_channel() {}

            void trigger_read() throw();
            void trigger_write() throw();

        private:
            event_channel(const event_channel&);
            event_channel& operator=(const event_channel&);
        };
    }
}
