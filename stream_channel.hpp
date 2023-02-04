/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "serv_types.hpp"

#include <cstdlib>
#include <string>

namespace ft
{
    namespace serv
    {
        class stream_channel : public event_channel_base
        {
        private:
            std::size_t init_buf_capacity;

        public:
            stream_channel(ident_t ident, const std::string& host, int serv, std::size_t init_buf_capacity = 4096);
            virtual ~stream_channel();

        protected:
            void begin_read();

        private:
            stream_channel(const stream_channel&);
            stream_channel& operator=(const stream_channel&);
        };
    }
}
