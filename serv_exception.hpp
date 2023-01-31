/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include <cerrno>
#include <cstring>
#include <exception>

namespace ft
{
    namespace serv
    {
        typedef int error_t;

        class syscall_failed : public std::exception
        {
        private:
            error_t e;
            const char* s;

        public:
            syscall_failed() throw() : e(errno), s(const_cast<const char*>(std::strerror(this->e))) {}
            syscall_failed(error_t e) throw() : e(e), s(const_cast<const char*>(std::strerror(e))) {}
            syscall_failed(error_t e, const char* s) throw() : e(e), s(s) {}
            virtual ~syscall_failed() throw() {}
            virtual const char* what() const throw() { return this->s; }

            error_t error() const throw() { return this->e; }
        };
    }
}