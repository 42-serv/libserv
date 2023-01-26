/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "event_worker.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/mutex.hpp>

namespace ft
{
    namespace serv
    {
        class event_worker_group
        {
        public:
            typedef dynamic_array<ft::shared_ptr<event_worker> >::type loop_list;

        private:
            ft::mutex lock;
            loop_list::size_type index;
            loop_list loops;

        public:
            event_worker_group();
            ~event_worker_group();

            void put_worker(const ft::shared_ptr<event_worker>& worker);
            const ft::shared_ptr<event_worker>& next();

        private:
            event_worker_group(const event_worker_group&);
            event_worker_group& operator=(const event_worker_group&);
        };
    }
}