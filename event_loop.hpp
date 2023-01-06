/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include "channel_base.hpp"
#include "task_base.hpp"

#include "pthread_wrapper.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class event_loop
        {
        public:
            typedef fast_dictionary<ident_t, ft::shared_ptr<channel_base> >::type channel_dictionary;
            typedef dynamic_array<ft::shared_ptr<task_base> >::type task_list;

        private:
            ident_t boss_fd;
            void* boss_list;
            channel_dictionary channels;
            task_list tasks;
            ::pthread_mutex_t lock;

        public:
            event_loop();
            virtual ~event_loop();

            void add_channel(const ident_t ident, const ft::shared_ptr<channel_base>& channel);
            void remove_channel(const ident_t ident);
            void watch_writability(const ident_t ident, const bool enable);
            void offer_task(const ft::shared_ptr<task_base> task);
            void loop();
            void wake_up();

        private:
            event_loop(const event_loop&);
            event_loop& operator=(const event_loop&);

            void process_events(void* list, int n) throw();
            void execute_tasks() throw();
        };
    }
}
