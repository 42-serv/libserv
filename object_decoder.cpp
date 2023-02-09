/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "object_decoder.hpp"

#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::object_decoder::object_decoder() : cumulative_obj()
{
}

ft::serv::object_decoder::~object_decoder()
{
}

void ft::serv::object_decoder::on_read(event_layer& layer, ft::shared_ptr<void> obj)
{
    assert(this->output.empty());

    this->cumulative_obj.push_back(obj);

    do
    {
        const size_type consumed_size = this->decode(this->cumulative_obj, this->output);
        if (consumed_size == 0)
        {
            assert(this->output.empty());

            break;
        }

        this->cumulative_obj.erase(this->cumulative_obj.begin(), this->cumulative_obj.begin() + consumed_size);
        for (output_buffer::iterator it = this->output.begin(); it != this->output.end(); ++it)
        {
            layer.notify_read(*it);
        }
        this->output.clear();
    } while (!0);
}

ft::serv::object_decoder::size_type ft::serv::object_decoder::decode(cumulative_list& obj, output_buffer& out)
{
    throw std::runtime_error("not implemented decode()");
}
