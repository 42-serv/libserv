/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#define null NULL
#define staticassert(_x) static_cast<void>(sizeof(char[(_x) ? 1 : -1]))
#define beginof(_x) (&(_x)[0])
#define countof(_x) (sizeof(_x) / sizeof((_x)[0]))

#include <map>
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

        template <typename K, typename V>
        struct fast_dictionary
        {
            typedef std::map<K, V> type; // unordered_map, flat_map
        };
    }
}
