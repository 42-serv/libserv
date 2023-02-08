/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bytes_decoder.hpp"

#include "event_layer.hpp"

ft::serv::bytes_decoder::bytes_decoder() : cumulative_buf() {}
ft::serv::bytes_decoder::~bytes_decoder() {}

void ft::serv::bytes_decoder::on_active(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_decoder::on_read(event_layer& layer, ft::shared_ptr<void>) { static_cast<void>(layer); }
void ft::serv::bytes_decoder::on_read_complete(event_layer& layer) { static_cast<void>(layer); }
void ft::serv::bytes_decoder::on_error(event_layer& layer, ft::shared_ptr<const std::exception>) { static_cast<void>(layer); }
void ft::serv::bytes_decoder::on_inactive(event_layer& layer) { static_cast<void>(layer); }
