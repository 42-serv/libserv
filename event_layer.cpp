/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_layer.hpp"

#include "event_channel.hpp"
#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <exception>

ft::serv::event_layer::event_layer(event_channel& channel, event_layer* next, event_layer* prev, const ft::shared_ptr<event_handler_base>& handler)
    : channel(channel), next(next), prev(prev), handler(handler) {}

ft::serv::event_layer::~event_layer() {}

// TODO: invoke

void ft::serv::event_layer::notify_active()
{
    this->handler->on_active(*this->next);
}

void ft::serv::event_layer::notify_read(void* arg)
{
    this->handler->on_read(*this->next, arg);
}

void ft::serv::event_layer::notify_error(const std::exception& e)
{
    this->handler->on_error(*this->next, e);
}

void ft::serv::event_layer::notify_inactive()
{
    this->handler->on_inactive(*this->next);
}

void ft::serv::event_layer::post_write(void* arg)
{
    this->handler->on_write(*this->prev, arg);
}

void ft::serv::event_layer::post_flush()
{
    this->handler->on_flush(*this->prev);
}

void ft::serv::event_layer::post_disconnect()
{
    this->handler->on_disconnect(*this->prev);
}
