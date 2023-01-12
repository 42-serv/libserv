/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <iostream>
#include <string>

#define FT_SERV_WRITE_LOG(msg) std::cout << msg << std::endl

namespace ft
{
    namespace serv
    {
        struct logger
        {
            static inline void trace(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            static inline void debug(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            static inline void info(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            static inline void warn(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            static inline void error(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }
        };
    }
}