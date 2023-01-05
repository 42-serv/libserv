/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <vector>

namespace ft
{
    namespace serv
    {
        typedef int ident_t;

        typedef unsigned char byte_t;

        template <typename T>
        struct dynamic_array
        {
            typedef std::vector<T> type;
        };
    }
}
