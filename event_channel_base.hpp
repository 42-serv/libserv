/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "byte_buffer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/weak_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class event_worker;

        class event_layer;

        class event_handler_base;

        class event_channel_base
        {
        private:
            ident_t ident;
            // local_address
            // remote_address
            byte_buffer send_buf;
            ft::shared_ptr<event_layer> pipeline_head;
            ft::shared_ptr<event_layer> pipeline_tail;
            event_worker* loop;

        public:
            bool readability_interested;
            bool writability_interested;
            bool readability_enabled;
            bool writability_enabled;
            bool input_closed;
            bool output_closed;

        public:
            event_channel_base(ident_t ident);
            virtual ~event_channel_base();

            ident_t get_ident() const throw() { return this->ident; }

            event_worker* get_loop() const;
            void set_loop(event_worker*);

            void trigger_read() throw();
            void trigger_write() throw();

            void add_first_handler(const ft::shared_ptr<event_handler_base>&);
            void add_last_handler(const ft::shared_ptr<event_handler_base>&);

            virtual void begin_read();

            void do_register(const ft::shared_ptr<event_channel_base>&);
            void do_write(const ft::serv::byte_buffer&);
            void do_flush();
            void do_disconnect();
            void do_deregister();

        private:
            event_channel_base(const event_channel_base&);
            event_channel_base& operator=(const event_channel_base&);
        };
    }
}
