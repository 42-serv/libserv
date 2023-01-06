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
            inline static void trace(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            inline static void debug(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            inline static void info(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            inline static void warn(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }

            inline static void error(const std::string& msg)
            {
                FT_SERV_WRITE_LOG(msg);
            }
        };
    }
}