/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_channel_base.hpp"

#include "byte_buffer.hpp"
#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "event_worker.hpp"
#include "serv_types.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

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
    }

    void on_write(ft::serv::event_layer& layer, ft::shared_ptr<const void> arg)
    {
        static_cast<void>(layer);

        ft::shared_ptr<const ft::serv::byte_buffer> buffer = ft::static_pointer_cast<const ft::serv::byte_buffer>(arg);
        this->channel.on_write(*buffer);
    }

    void on_flush(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.on_flush();
    }

    void on_disconnect(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.on_disconnect();
    }

    void on_deregister(ft::serv::event_layer& layer)
    {
        static_cast<void>(layer);

        this->channel.get_loop()->remove_channel(this->channel.get_ident());
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

ft::serv::event_channel_base::event_channel_base(ident_t ident)
    : ident(ident),
      send_buf(),
      pipeline_head(),
      pipeline_tail(),
      loop(),
      readability_interested(),
      writability_interested(),
      readability_enabled(),
      writability_enabled(),
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
}

ft::serv::ident_t ft::serv::event_channel_base::get_ident() const throw()
{
    return this->ident;
}

ft::shared_ptr<ft::serv::event_worker> ft::serv::event_channel_base::get_loop() const
{
    if (ft::shared_ptr<ft::serv::event_worker> ptr = this->loop.lock())
    {
        return ptr;
    }

    throw std::runtime_error("bad_loop");
}

void ft::serv::event_channel_base::set_loop(ft::shared_ptr<event_worker> loop)
{
    assert(this->loop.expired() || !loop);

    this->loop = loop;
}

void ft::serv::event_channel_base::trigger_read() throw()
{
    // FIXME: ...
}

void ft::serv::event_channel_base::trigger_write() throw()
{
    // FIXME: ...
}

void ft::serv::event_channel_base::do_register()
{
    this->pipeline_tail->post_register();
    this->pipeline_head->notify_active();
}

void ft::serv::event_channel_base::do_deregister()
{
    this->pipeline_head->notify_inactive();
    this->pipeline_tail->post_deregister();
}

void ft::serv::event_channel_base::add_first_handler(const ft::shared_ptr<event_handler_base>& handler)
{
    ft::shared_ptr<event_layer> layer = ft::make_shared<event_layer>(*this, handler);
    this->pipeline_head->set_next(layer);
}

void ft::serv::event_channel_base::add_last_handler(const ft::shared_ptr<event_handler_base>& handler)
{
    ft::shared_ptr<event_layer> layer = ft::make_shared<event_layer>(*this, handler);
    this->pipeline_tail->set_prev(layer);
}

void ft::serv::event_channel_base::on_write(const ft::serv::byte_buffer&)
{
    // FIXME: put_data
}

void ft::serv::event_channel_base::on_flush()
{
    // FIXME: send
}

void ft::serv::event_channel_base::on_disconnect()
{
    // FIXME: force (if is not called by deregister then try deregister)
}

void ft::serv::event_channel_base::begin_read()
{
    // FIXME: ... implement in child class (server, stream)
}
