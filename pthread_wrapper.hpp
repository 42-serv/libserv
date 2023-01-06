/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "pthread.h"

namespace ft
{
    class pthread_mutex_guard
    {
    private:
        pthread_mutex_t& mutex;

    public:
        inline pthread_mutex_guard(pthread_mutex_t& mutex) : mutex(mutex) { ::pthread_mutex_lock(&mutex); }
        inline ~pthread_mutex_guard() { ::pthread_mutex_unlock(&this->mutex); }

    private:
        pthread_mutex_guard(const pthread_mutex_guard&);
        pthread_mutex_guard& operator=(const pthread_mutex_guard&);
    };
}
