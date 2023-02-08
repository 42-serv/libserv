/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class object_encoder : public event_handler_base
        {
        public:
            object_encoder();
            ~object_encoder();

            void on_write(event_layer& layer, ft::shared_ptr<const void>);
            void on_flush(event_layer& layer);

        private:
            object_encoder(const object_encoder&);
            object_encoder& operator=(const object_encoder&);
        };
    }
}
