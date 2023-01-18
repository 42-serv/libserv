/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_layer.hpp"

#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::event_layer::event_layer(event_layer* next, event_layer* prev, const ft::shared_ptr<event_handler_base>& handler)
    : next(next), prev(prev), handler(handler) {}

ft::serv::event_layer::~event_layer() {}

void ft::serv::event_layer::notify_active()
{
    this->handler->on_active(*this);
    this->next->notify_active();
}

void ft::serv::event_layer::notify_read()
{
    this->handler->on_read(*this);
    this->next->notify_read();
}

void ft::serv::event_layer::notify_error()
{
    this->handler->on_error(*this);
    this->next->notify_error();
}

void ft::serv::event_layer::notify_inactive()
{
    this->handler->on_inactive(*this);
    this->next->notify_inactive();
}

void ft::serv::event_layer::post_write()
{
    this->prev->post_write();
}

void ft::serv::event_layer::post_flush()
{
    this->prev->post_flush();
}

void ft::serv::event_layer::post_disconnect()
{
    this->prev->post_disconnect();
}
