/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "server_bootstrap.hpp"

#include "event_worker_group.hpp"
#include "serv_types.hpp"
#include "server_channel.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <exception>
#include <string>

ft::serv::server_bootstrap::server_bootstrap(const ft::shared_ptr<event_worker_group>& boss_group, const ft::shared_ptr<event_worker_group>& child_group, make_server_t make_server)
    : boss_group(boss_group),
      child_group(child_group),
      make_server(make_server)
{
    boss_group->wait_all();
    child_group->wait_all();
}

ft::serv::server_bootstrap::~server_bootstrap()
{
    boss_group->join_all();
    child_group->join_all();
}

bool ft::serv::server_bootstrap::start_server(const std::string& host_str, const std::string& serv_str)
{
    const ident_t boss_ident = socket_utils::bind_socket(host_str.c_str(), serv_str.c_str());
    if (boss_ident < 0)
    {
        return false;
    }

    try
    {
        socket_utils::set_nonblocking(boss_ident, true);
        socket_utils::listen_socket(boss_ident);
    }
    catch (const std::exception& e)
    {
        socket_utils::close_socket(boss_ident);
        throw;
    }

    std::string host;
    int serv;
    socket_utils::name_socket(boss_ident, host, serv);
    ft::shared_ptr<event_channel_base> boss = (*this->make_server)(boss_ident, host, serv, this->child_group);
    boss->set_loop(this->boss_group->next());
    boss->do_register();

    return true;
}
