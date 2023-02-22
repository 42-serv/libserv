/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_channel_base.hpp"

#include "byte_buffer.hpp"
#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "event_worker.hpp"
#include "logger.hpp"
#include "serv_types.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

#include <cassert>
#include <stdexcept>
#include <string>

class dummy_inbound_adapter : public ft::serv::event_handler_base
{
private:
    ft::serv::event_channel_base& channel;

public:
    dummy_inbound_adapter(ft::serv::event_channel_base& channel) : ft::serv::event_handler_base(), channel(channel) {}
    ~dummy_inbound_adapter() {}

    void on_register(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.get_loop()->add_channel(this->channel.shared_from_this());
        this->channel.get_pipeline()->notify_active();
    }

    void on_write(ft::serv::event_layer& layer, ft::shared_ptr<const void> arg)
    {
        static_cast<void>(layer);

        ft::shared_ptr<const ft::serv::byte_buffer> buffer = ft::static_pointer_cast<const ft::serv::byte_buffer>(arg);
        this->channel.write(*buffer);
    }

    void on_flush(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.flush();
    }

    void on_finish(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.finish();
    }

    void on_disconnect(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.shutdown_output();
        this->channel.shutdown_input();
    }

private:
    dummy_inbound_adapter(const dummy_inbound_adapter&);
    dummy_inbound_adapter& operator=(const dummy_inbound_adapter&);
};

class dummy_outbound_adapter : public ft::serv::event_handler_base
{
public:
    dummy_outbound_adapter() {}
    ~dummy_outbound_adapter() {}

    void on_active(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);
        throw std::runtime_error("unhandled on_active");
    }

    void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
    {
        static_cast<void>(layer), static_cast<void>(arg);
        throw std::runtime_error("unhandled on_read");
    }

    void on_read_complete(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);
        throw std::runtime_error("unhandled on_read_complete");
    }

    void on_error(ft::serv::event_layer& layer, ft::shared_ptr<const std::exception> e)
    {
        static_cast<void>(layer), static_cast<void>(e);
        throw std::runtime_error("unhandled on_error");
    }

    void on_inactive(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);
        throw std::runtime_error("unhandled on_inactive");
    }

private:
    dummy_outbound_adapter(const dummy_outbound_adapter&);
    dummy_outbound_adapter& operator=(const dummy_outbound_adapter&);
};

ft::serv::event_channel_base::event_channel_base(ident_t ident, const std::string& host, int serv)
    : ident(ident),
      host(host),
      serv(serv),
      pipeline_head(),
      pipeline_tail(),
      written_buf(),
      flushed_buf(),
      loop(),
      readability_interested(),
      writability_interested(),
      readability_enabled(),
      writability_enabled(),
      finished(),
      input_closed(),
      output_closed()
{
    this->pipeline_head = ft::make_shared<event_layer>(*this, ft::make_shared<dummy_inbound_adapter>(*this));
    this->pipeline_tail = ft::make_shared<event_layer>(*this, ft::make_shared<dummy_outbound_adapter>());
    this->pipeline_head->set_next(this->pipeline_tail);
    this->pipeline_tail->set_prev(this->pipeline_head);
}

ft::serv::event_channel_base::~event_channel_base()
{
    socket_utils::close_socket(this->ident);
}

ft::serv::ident_t ft::serv::event_channel_base::get_ident() const throw()
{
    return this->ident;
}

const std::string& ft::serv::event_channel_base::get_host() const throw()
{
    return this->host;
}

int ft::serv::event_channel_base::get_serv() const throw()
{
    return this->serv;
}

const ft::shared_ptr<ft::serv::event_layer>& ft::serv::event_channel_base::get_pipeline() const throw()
{
    return this->pipeline_head;
}

void ft::serv::event_channel_base::add_first_handler(const ft::shared_ptr<event_handler_base>& handler)
{
    const ft::shared_ptr<event_layer> layer = ft::make_shared<event_layer>(*this, handler);
    this->pipeline_head->set_next(layer);
}

void ft::serv::event_channel_base::add_last_handler(const ft::shared_ptr<event_handler_base>& handler)
{
    const ft::shared_ptr<event_layer> layer = ft::make_shared<event_layer>(*this, handler);
    this->pipeline_tail->set_prev(layer);
}

ft::shared_ptr<ft::serv::event_worker> ft::serv::event_channel_base::get_loop() const
{
    if (ft::shared_ptr<ft::serv::event_worker> ptr = this->loop.lock())
    {
        return ptr;
    }

    throw std::runtime_error("bad_loop");
}

void ft::serv::event_channel_base::set_loop(const ft::shared_ptr<event_worker>& loop)
{
    assert(this->loop.expired() || !loop);

    this->loop = loop;
}

void ft::serv::event_channel_base::loop_register()
{
    this->readability_interested = true;
    this->writability_interested = true;
    this->pipeline_tail->post_register();
}

void ft::serv::event_channel_base::load_interested(bool out_interested[2], bool out_changed[2]) const throw()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    out_interested[0] = this->readability_interested;
    out_interested[1] = this->writability_interested;
    out_changed[0] = this->readability_enabled != this->readability_interested;
    out_changed[1] = this->writability_enabled != this->writability_interested;
}

void ft::serv::event_channel_base::store_interested() throw()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    this->readability_enabled = this->readability_interested;
    this->writability_enabled = this->writability_interested;
}

void ft::serv::event_channel_base::trigger_read() throw()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    try
    {
        this->begin_read();
    }
    catch (const std::exception& e)
    {
        const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline();
        try
        {
            pipeline->notify_error(ft::shared_ptr<const std::exception>(&e, ft::nothing()));
        }
        catch (...)
        {
            logger::error("double fault on trigger_read");
        }
    }
}

void ft::serv::event_channel_base::trigger_write() throw()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    try
    {
        this->begin_write();
    }
    catch (const std::exception& e)
    {
        const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline();
        try
        {
            pipeline->notify_error(ft::shared_ptr<const std::exception>(&e, ft::nothing()));
        }
        catch (...)
        {
            logger::error("double fault on trigger_write");
        }
    }
}

void ft::serv::event_channel_base::write(const ft::serv::byte_buffer& buf)
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    if (this->finished)
    {
        return;
    }
    this->written_buf.append_from(buf);
}

void ft::serv::event_channel_base::flush()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    if (this->finished)
    {
        return;
    }
    byte_buffer& buf = this->written_buf;
    this->flushed_buf.append_from(buf);
    buf.clear();
    this->begin_write();
}

void ft::serv::event_channel_base::finish()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    if (this->finished)
    {
        return;
    }
    this->finished = true;
}

void ft::serv::event_channel_base::shutdown_input()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    if (this->input_closed)
    {
        return;
    }
    this->input_closed = true;
    this->shutdown_half(true);
}

void ft::serv::event_channel_base::shutdown_output()
{
    assert(!this->loop.expired() && this->loop.lock()->is_in_event_loop());

    if (this->output_closed)
    {
        return;
    }
    this->output_closed = true;
    this->shutdown_half(false);
    socket_utils::finish_socket(this->get_ident());
}

void ft::serv::event_channel_base::begin_write()
{
    const ft::shared_ptr<event_worker>& worker = this->get_loop();
    const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline();
    bool not_yet_completed = false;
    byte_buffer& buf = this->flushed_buf;
    while (!buf.empty())
    {
        const long len = socket_utils::send_socket(this->get_ident(), buf.get(), buf.size());
        if (len < 0)
        {
            const error_t err = -len;
            if (err == EAGAIN)
            {
                not_yet_completed = true;
                break;
            }
            pipeline->notify_error(ft::make_shared<syscall_failed>(err));
            return;
        }
        buf.remove(len);
    }
    buf.discard();
    this->writability_interested = not_yet_completed;
    worker->watch_ability(*this);
    if (this->finished && !this->writability_interested)
    {
        this->shutdown_output();
    }
}

void ft::serv::event_channel_base::shutdown_half(bool input_or_output)
{
    const ft::shared_ptr<event_worker>& worker = this->get_loop();
    const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline();
    if (this->input_closed && this->output_closed)
    {
        pipeline->notify_inactive();
        worker->remove_channel(this->get_ident());
    }
    else
    {
        if (input_or_output)
        {
            this->readability_interested = false;
        }
        else
        {
            this->writability_interested = false;
        }
        worker->watch_ability(*this);
    }
}
