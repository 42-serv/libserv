/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "object_encoder.hpp"

#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::object_encoder::object_encoder()
    : temp()
{
}

ft::serv::object_encoder::~object_encoder()
{
}

void ft::serv::object_encoder::on_write(event_layer& layer, ft::shared_ptr<const void> arg)
{
    assert(this->temp.empty());

    this->encode(arg, this->temp);
    for (output_buffer::iterator it = this->temp.begin(); it != this->temp.end(); ++it)
    {
        layer.post_write(*it);
    }
    this->temp.clear();
}

void ft::serv::object_encoder::encode(const ft::shared_ptr<const void>&, output_buffer&)
{
    throw std::runtime_error("not implemented encode()");
}
