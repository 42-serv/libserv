/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_handler_base.hpp"

#include "event_layer.hpp"
#include "serv_types.hpp"

ft::serv::event_handler_base::event_handler_base() {}

ft::serv::event_handler_base::~event_handler_base() {}

void ft::serv::event_handler_base::on_active(event_layer& next)
{
    next.notify_active();
}

void ft::serv::event_handler_base::on_read(event_layer& next, void* arg)
{
    next.notify_read(arg);
}

void ft::serv::event_handler_base::on_error(event_layer& next, const std::exception& e)
{
    next.notify_error(e);
}

void ft::serv::event_handler_base::on_inactive(event_layer& next)
{
    next.notify_inactive();
}

void ft::serv::event_handler_base::on_write(event_layer& prev, void* arg)
{
    prev.post_write(arg);
}

void ft::serv::event_handler_base::on_flush(event_layer& prev)
{
    prev.post_flush();
}

void ft::serv::event_handler_base::on_disconnect(event_layer& prev)
{
    prev.post_disconnect();
}
