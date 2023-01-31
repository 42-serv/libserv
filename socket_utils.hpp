/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            struct gai_guard
            {
                struct ::addrinfo* result;

                gai_guard(const char* host, const char* serv, struct ::addrinfo& hints)
                {
                    int gai_errno = ::getaddrinfo(host, serv, &hints, &result);
                    if (gai_errno != 0)
                    {
                        throw syscall_failed(gai_errno, ::gai_strerror(gai_errno));
                    }
                }

                ~gai_guard()
                {
                    ::freeaddrinfo(result);
                }

            private:
                gai_guard(const gai_guard&);
                gai_guard& operator=(const gai_guard&);
            };
        }

        struct socket_utils
        {
            static inline ident_t bind_socket(const char* host, const char* serv)
            {
                struct ::addrinfo hints;
                ::memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = 0; // ANY

                // for bind()
                hints.ai_flags |= AI_PASSIVE;

                const _internal::gai_guard gai(host, serv, hints);

                ident_t socket;
                struct ::addrinfo* it;
                for (it = gai.result; it != null; it = it->ai_next)
                {
                    socket = ::socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                    if (socket < 0)
                    {
                        throw syscall_failed();
                    }

                    if (::bind(socket, it->ai_addr, it->ai_addrlen) < 0)
                    {
                        ::close(socket);
                        continue;
                    }

                    break;
                }

                if (it == null)
                {
                    return -1;
                }

                return socket;
            }

            // TODO: listen_socket

            // TODO: accept_socket

            static inline long recv_socket(const ident_t socket, void* buf, std::size_t len)
            {
                staticassert(sizeof(long) >= sizeof(::ssize_t));

                ::ssize_t value = ::recv(socket, buf, len, 0);
                if (value < 0)
                {
                    const syscall_failed e;
                    return -e.error();
                }
                return value;
            }

            // TODO: send_socket

            // TODO: shutdown_socket: `::shutdown(socket, SHUT_RDWR);`

            static inline void close_socket(const ident_t socket)
            {
                if (::close(socket) < 0)
                {
                    const syscall_failed e;
                    // ignore
                }
            }

            static inline void set_socket_reuse_address(const ident_t socket, const bool value)
            {
                if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline void set_socket_linger(const ident_t socket, const bool enable, const int time_in_seconds)
            {
                struct linger value;
                value.l_onoff = enable;
                value.l_linger = time_in_seconds;

                if (::setsockopt(socket, SOL_SOCKET, SO_LINGER, &value, sizeof(value)) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline void set_tcp_nodelay(const ident_t socket, const bool value)
            {
                if (::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline void set_nonblocking(const ident_t socket, const bool value)
            {
                int status_flags = ::fcntl(socket, F_GETFL, 0);
                if (status_flags < 0)
                {
                    throw syscall_failed();
                }

                if (value)
                {
                    status_flags |= O_NONBLOCK;
                }
                else
                {
                    status_flags &= ~O_NONBLOCK;
                }

                if (::fcntl(socket, F_SETFL, status_flags) < 0)
                {
                    throw syscall_failed();
                }
            }
        };
    }
}
