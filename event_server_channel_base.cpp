/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_server_channel_base.hpp"

#include "event_layer.hpp"
#include "event_stream_channel.hpp"
#include "event_worker_group.hpp"
#include "serv_types.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

#include <exception>
#include <string>

ft::serv::event_server_channel_base::event_server_channel_base(ident_t ident, const std::string& host, int serv, const ft::shared_ptr<event_worker_group>& group)
    : event_channel_base(ident, host, serv),
      group(group)
{
}

ft::serv::event_server_channel_base::~event_server_channel_base()
{
}

void ft::serv::event_server_channel_base::begin_read()
{
    const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline_head();
    const ft::shared_ptr<event_layer>& pipeline_back = this->get_pipeline_tail();
    while (!0)
    {
        std::string child_host;
        int child_serv;
        const ident_t child_ident = socket_utils::accept_socket(this->get_ident(), child_host, child_serv);
        if (child_ident < 0)
        {
            const error_t err = -child_ident;
            if (err == EAGAIN)
            {
                break;
            }
            pipeline->notify_error(ft::make_shared<syscall_failed>(err));
            pipeline_back->post_disconnect();
            return;
        }
        const ft::shared_ptr<ft::serv::event_channel_base> child = this->make_child(child_ident, child_host, child_serv);
        ft::serv::socket_utils::set_nonblocking(child_ident, true);
        pipeline->notify_read(child);
        child->set_loop(this->group->next());
        child->do_register();
    }
    pipeline->notify_read_complete();
}

ft::shared_ptr<ft::serv::event_channel_base> ft::serv::event_server_channel_base::make_child(ident_t child_ident, const std::string& child_host, int child_serv)
{
    return ft::make_shared<event_stream_channel>(child_ident, child_host, child_serv);
}
