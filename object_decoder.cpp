/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "object_decoder.hpp"

#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::object_decoder::object_decoder() : cumulative_obj() {}
ft::serv::object_decoder::~object_decoder() {}

void ft::serv::object_decoder::on_read(event_layer& layer, ft::shared_ptr<void>) { static_cast<void>(layer); }
void ft::serv::object_decoder::on_read_complete(event_layer& layer) { static_cast<void>(layer); }
