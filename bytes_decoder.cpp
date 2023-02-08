/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bytes_decoder.hpp"

#include "byte_buffer.hpp"
#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::bytes_decoder::bytes_decoder() : cumulative_buf() {}
ft::serv::bytes_decoder::~bytes_decoder() {}

void ft::serv::bytes_decoder::on_read(event_layer& layer, ft::shared_ptr<void> arg)
{
    const ft::shared_ptr<byte_buffer> buf = ft::static_pointer_cast<byte_buffer>(arg);
}

void ft::serv::bytes_decoder::on_read_complete(event_layer& layer)
{
    this->cumulative_buf.discard();
}
