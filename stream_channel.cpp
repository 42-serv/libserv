/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "stream_channel.hpp"

#include "byte_buffer.hpp"
#include "event_layer.hpp"
#include "event_worker_group.hpp"
#include "serv_types.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

#include <string>

ft::serv::stream_channel::stream_channel(ident_t ident, const std::string& host, int serv, std::size_t init_buf_capacity)
    : event_channel_base(ident, host, serv),
      init_buf_capacity(init_buf_capacity)
{
}

ft::serv::stream_channel::~stream_channel()
{
}

void ft::serv::stream_channel::begin_read()
{
    const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline_head();
    const ft::shared_ptr<event_layer>& pipeline_back = this->get_pipeline_tail();
    bool orderly_shutdown = false;
    while (!0)
    {
        const ft::shared_ptr<byte_buffer> buf = ft::make_shared<byte_buffer>(this->init_buf_capacity);
        const long len = socket_utils::recv_socket(this->get_ident(), buf->raw_buffer(), buf->raw_length());
        if (len < 0)
        {
            const error_t err = -len;
            if (err == EAGAIN)
            {
                break;
            }
            pipeline->notify_error(ft::make_shared<syscall_failed>(err));
            pipeline_back->post_disconnect();
            return;
        }
        if (len == 0)
        {
            orderly_shutdown = true;
            break;
        }
        buf->raw_shrink(len);
        pipeline->notify_read(buf);
    }
    pipeline->notify_read_complete();
    if (orderly_shutdown)
    {
        // NOTE: handle orderly shutdown
        // this->input_closed = true;
        pipeline_back->post_disconnect(); // FIXME: temporary
    }
}
