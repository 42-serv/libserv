/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel.hpp"
#include "serv_types.hpp"
#include "task_base.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/mutex.hpp>

namespace ft
{
    namespace serv
    {
        class event_worker
        {
        public:
            typedef fast_dictionary<ident_t, ft::shared_ptr<event_channel> >::type channel_dictionary;
            typedef dynamic_array<ft::shared_ptr<task_base> >::type task_list;

        private:
            ft::mutex lock;
            ident_t boss_fd;
            void* boss_list;
            channel_dictionary channels;
            task_list tasks;

        public:
            event_worker();
            virtual ~event_worker();

            void add_channel(const ident_t ident, const ft::shared_ptr<event_channel>& channel);
            void remove_channel(const ident_t ident);
            void watch_writability(const ident_t ident, const bool enable);
            void offer_task(const ft::shared_ptr<task_base> task);
            void loop();
            void wake_up();

        private:
            event_worker(const event_worker&);
            event_worker& operator=(const event_worker&);

            void process_events(void* list, int n) throw();
            void execute_tasks() throw();
        };
    }
}
