/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class object_decoder : public event_handler_base
        {
        public:
            typedef dynamic_array<ft::shared_ptr<void> >::type cumulative_list;
            typedef dynamic_array<ft::shared_ptr<void> >::type output_buffer;
            typedef cumulative_list::size_type size_type;

        private:
            cumulative_list cumulative_obj;
            output_buffer output;

        public:
            object_decoder();
            ~object_decoder();

            void on_read(event_layer& layer, ft::shared_ptr<void>);
            void on_read_complete(event_layer& layer);

            virtual size_type decode(cumulative_list& obj, output_buffer& out);

        private:
            object_decoder(const object_decoder&);
            object_decoder& operator=(const object_decoder&);
        };
    }
}
