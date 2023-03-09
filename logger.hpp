/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <cstdarg>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include <cassert>

#define FT_SERV_WRITE_LOG(msg) std::cout << msg

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            inline std::string make_utc_string()
            {
                std::time_t time;
                assert(std::time(&time) != static_cast<std::time_t>(-1));
                char time_str[sizeof("yyyy-mm-ddThh:mm:ssZ")];
                assert(std::strftime(time_str, sizeof(time_str), "%FT%TZ", std::gmtime(&time)) == sizeof(time_str) - sizeof('\0'));
                return time_str;
            }

            inline std::string make_format_string(const char* format, va_list& ap)
            {
                std::ostringstream oss;

                for (; *format != '\0'; format++)
                {
                    if (*format == '%')
                    {
                        format++;
                        if (*format == '\0')
                        {
                            break;
                        }

                        switch (*format)
                        {
                        case 'd':
                            oss << std::dec << va_arg(ap, int);
                            break;
                        case 'u':
                            oss << std::dec << va_arg(ap, unsigned int);
                            break;
                        case 'x':
                            oss << std::hex << va_arg(ap, unsigned int);
                            break;
                        case 'f':
                            oss << std::fixed << va_arg(ap, double);
                            break;
                        case 'c':
                            oss << static_cast<char>(va_arg(ap, int));
                            break;
                        case 's':
                            oss << va_arg(ap, const char*);
                            break;
                        case 'p':
                            oss << std::showbase << va_arg(ap, void*);
                            break;
                        }
                    }
                    else
                    {
                        oss << *format;
                    }
                }
                return oss.str();
            }
        }

        struct logger
        {
            static inline void trace(const char* format, ...)
            {
                va_list ap;
                va_start(ap, format);
                std::string msg = "[" + _internal::make_utc_string() + "] [TRACE] " + _internal::make_format_string(format, ap) + "\n";
                va_end(ap);

                FT_SERV_WRITE_LOG(msg);
            }

            static inline void debug(const char* format, ...)
            {
                va_list ap;
                va_start(ap, format);
                std::string msg = "[" + _internal::make_utc_string() + "] [DEBUG] " + _internal::make_format_string(format, ap) + "\n";
                va_end(ap);

                FT_SERV_WRITE_LOG(msg);
            }

            static inline void info(const char* format, ...)
            {
                va_list ap;
                va_start(ap, format);
                std::string msg = "[" + _internal::make_utc_string() + "] [INFO] " + _internal::make_format_string(format, ap) + "\n";
                va_end(ap);

                FT_SERV_WRITE_LOG(msg);
            }

            static inline void warn(const char* format, ...)
            {
                va_list ap;
                va_start(ap, format);
                std::string msg = "[" + _internal::make_utc_string() + "] [WARN] " + _internal::make_format_string(format, ap) + "\n";
                va_end(ap);

                FT_SERV_WRITE_LOG(msg);
            }

            static inline void error(const char* format, ...)
            {
                va_list ap;
                va_start(ap, format);
                std::string msg = "[" + _internal::make_utc_string() + "] [ERROR] " + _internal::make_format_string(format, ap) + "\n";
                va_end(ap);

                FT_SERV_WRITE_LOG(msg);
            }
        };
    }
}