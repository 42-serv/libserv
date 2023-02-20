/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "byte_buffer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/enable_shared_from_this.hpp>
#include <smart_ptr/shared_ptr.hpp>
#include <smart_ptr/weak_ptr.hpp>

#include <string>

namespace ft
{
    namespace serv
    {
        class event_worker;

        class event_layer;

        class event_handler_base;

        class event_channel_base : public ft::enable_shared_from_this<event_channel_base>
        {
        private:
            ident_t ident;
            std::string host;
            int serv;
            ft::shared_ptr<event_layer> pipeline_head;
            ft::shared_ptr<event_layer> pipeline_tail;
            byte_buffer written_buf;
            byte_buffer flushed_buf;
            ft::weak_ptr<event_worker> loop;

        private:
            bool readability_interested;
            bool writability_interested;
            bool readability_enabled;
            bool writability_enabled;
            bool input_closed;
            bool output_closed;

        public:
            event_channel_base(ident_t ident, const std::string& host, int serv);
            virtual ~event_channel_base();

            ident_t get_ident() const throw();
            const std::string& get_host() const throw();
            int get_serv() const throw();

            const ft::shared_ptr<event_layer>& get_pipeline() const throw();
            void add_first_handler(const ft::shared_ptr<event_handler_base>&);
            void add_last_handler(const ft::shared_ptr<event_handler_base>&);

            ft::shared_ptr<ft::serv::event_worker> get_loop() const;
            void set_loop(const ft::shared_ptr<event_worker>&);
            void loop_register();

            void load_interested(bool out_interested[2], bool out_changed[2]) const throw();
            void store_interested() throw();

            void trigger_read() throw();
            void trigger_write() throw();

            void write(const ft::serv::byte_buffer&);
            void flush();

            void shutdown_input();
            void shutdown_output();

        protected:
            virtual void begin_read() = 0;
            virtual void begin_write();

        private:
            event_channel_base(const event_channel_base&);
            event_channel_base& operator=(const event_channel_base&);
        };
    }
}
