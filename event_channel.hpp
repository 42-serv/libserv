/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "byte_buffer.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            class dummy_head_layer : public ft::serv::event_layer
            {
            };

            class dummy_tail_layer : public ft::serv::event_layer
            {
            };
        }

        class event_channel
        {
        private:
            ident_t ident;
            byte_buffer recv_buf;
            byte_buffer send_buf;
            _internal::dummy_head_layer pipeline_head;
            _internal::dummy_tail_layer pipeline_tail;
            // local_address
            // remote_address

        public:
            bool readability_interested;
            bool writability_interested;
            bool readability_enabled;
            bool writability_enabled;
            bool input_closed;
            bool output_closed;

        public:
            event_channel(ident_t ident);
            virtual ~event_channel() {}

            ident_t get_ident() const throw() { return this->ident; }

            void trigger_read() throw();
            void trigger_write() throw();

        private:
            event_channel(const event_channel&);
            event_channel& operator=(const event_channel&);
        };
    }
}
