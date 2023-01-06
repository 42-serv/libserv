/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

namespace ft
{
    namespace serv
    {
        class task_base
        {
        public:
            virtual ~task_base() {}

            virtual void run() throw() = 0;

        private:
            task_base(const task_base&);
            task_base& operator=(const task_base&);
        };
    }
}
