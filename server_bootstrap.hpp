/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <string>

namespace ft
{
    namespace serv
    {
        class event_worker_group;

        class server_bootstrap
        {
        public:
            typedef ft::shared_ptr<event_channel_base> (*make_server_t)(ident_t ident, const std::string& host, int serv, const ft::shared_ptr<event_worker_group>& child_group);

        private:
            ft::shared_ptr<event_worker_group> boss_group;
            ft::shared_ptr<event_worker_group> child_group;
            make_server_t make_server;
            dynamic_array<ft::shared_ptr<event_channel_base> >::type server_locker;

        public:
            server_bootstrap(const ft::shared_ptr<event_worker_group>& boss_group, const ft::shared_ptr<event_worker_group>& child_group, make_server_t make_server);
            ~server_bootstrap();

        public:
            void begin_server(const std::string& host_str, const std::string& serv_str);

        private:
            server_bootstrap(const server_bootstrap&);
            server_bootstrap& operator=(const server_bootstrap&);
        };
    }
}
