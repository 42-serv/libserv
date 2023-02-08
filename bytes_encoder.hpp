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
        class bytes_encoder : public event_handler_base
        {
        public:
            bytes_encoder();
            ~bytes_encoder();

            void on_write(event_layer& layer, ft::shared_ptr<const void>);
            void on_flush(event_layer& layer);

        private:
            bytes_encoder(const bytes_encoder&);
            bytes_encoder& operator=(const bytes_encoder&);
        };
    }
}
