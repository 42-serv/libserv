/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "duplex_adapter.hpp"

#include "event_layer.hpp"

ft::serv::duplex_adapter::duplex_adapter() {}
ft::serv::duplex_adapter::~duplex_adapter() {}

void ft::serv::duplex_adapter::on_active(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_read(event_layer& layer, ft::shared_ptr<void>) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_read_complete(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_error(event_layer& layer, ft::shared_ptr<const std::exception>) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_inactive(event_layer& layer) { static_cast<void>(layer); }

void ft::serv::duplex_adapter::on_register(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_write(event_layer& layer, ft::shared_ptr<const void>) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_flush(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_disconnect(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::duplex_adapter::on_deregister(event_layer& layer) { static_cast<void>(layer); }
