/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "event_stream_channel.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <string>

namespace ft
{
    namespace serv
    {
        class event_worker_group;

        class event_server_channel_base : public event_channel_base
        {
        private:
            ft::shared_ptr<event_worker_group> group;

        public:
            event_server_channel_base(ident_t ident, const std::string& host, int serv, const ft::shared_ptr<event_worker_group>& group);
            virtual ~event_server_channel_base();

        protected:
            void begin_read();
            virtual ft::shared_ptr<event_channel_base> make_child(ident_t child_ident, const std::string& child_host, int child_serv);

        private:
            event_server_channel_base(const event_server_channel_base&);
            event_server_channel_base& operator=(const event_server_channel_base&);
        };
    }
}
