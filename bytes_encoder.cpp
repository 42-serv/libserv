/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bytes_encoder.hpp"

#include "event_layer.hpp"

ft::serv::bytes_encoder::bytes_encoder() {}
ft::serv::bytes_encoder::~bytes_encoder() {}

void ft::serv::bytes_encoder::on_active(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_read(event_layer& layer, ft::shared_ptr<void>) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_read_complete(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_error(event_layer& layer, ft::shared_ptr<const std::exception>) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_inactive(event_layer& layer) { static_cast<void>(layer); }

void ft::serv::bytes_encoder::on_register(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_write(event_layer& layer, ft::shared_ptr<const void>) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_flush(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_disconnect(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_encoder::on_deregister(event_layer& layer) { static_cast<void>(layer); }
