/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "event_layer.hpp"

namespace ft
{
    namespace serv
    {
        class object_encoder : public event_handler_base
        {
        public:
            object_encoder();
            ~object_encoder();

            void on_active(event_layer& layer);
            void on_read(event_layer& layer, ft::shared_ptr<void>);
            void on_read_complete(event_layer& layer);
            void on_error(event_layer& layer, ft::shared_ptr<const std::exception>);
            void on_inactive(event_layer& layer);

            void on_register(event_layer& layer);
            void on_write(event_layer& layer, ft::shared_ptr<const void>);
            void on_flush(event_layer& layer);
            void on_disconnect(event_layer& layer);
            void on_deregister(event_layer& layer);

        private:
            object_encoder(const object_encoder&);
            object_encoder& operator=(const object_encoder&);
        };
    }
}
