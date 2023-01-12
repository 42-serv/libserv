/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

namespace ft
{
    namespace serv
    {
        static inline void close_socket(const ident_t sock)
        {
            if (::close(sock) < 0)
            {
                const syscall_failed e;
                // ignore
            }
        }

        static inline bool set_socket_reuse_address(const ident_t sock, const bool value)
        {
            return !(::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0);
        }

        static inline bool set_socket_linger(const ident_t sock, const bool enable, const int time_in_seconds)
        {
            struct linger value;
            value.l_onoff = enable;
            value.l_linger = time_in_seconds;

            return !(::setsockopt(sock, SOL_SOCKET, SO_LINGER, &value, sizeof(value)) < 0);
        }

        static inline bool set_tcp_nodelay(const ident_t sock, const bool value)
        {
            return !(::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) < 0);
        }

        static inline bool set_nonblocking(const ident_t sock, const bool value)
        {
            int status_flags = ::fcntl(sock, F_GETFL, 0);
            if (status_flags < 0)
            {
                return false;
            }

            if (value)
            {
                status_flags |= O_NONBLOCK;
            }
            else
            {
                status_flags &= ~O_NONBLOCK;
            }

            if (::fcntl(sock, F_SETFL, status_flags) < 0)
            {
                return false;
            }

            return true;
        }
    }
}
