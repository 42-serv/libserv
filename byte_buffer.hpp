/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include <cstring>

#include <cassert>

namespace ft
{
    namespace serv
    {
        class byte_buffer
        {
        public:
            typedef dynamic_array<byte_t>::type container_type;
            typedef container_type::size_type size_type;
            typedef container_type::iterator iterator;

        private:
            container_type buffer;
            size_type position;

        public:
            inline byte_buffer() : buffer(), position() {}

            inline const byte_t* raw_get() const throw()
            {
                return &this->buffer[this->position];
            }

            inline size_type raw_size() const throw()
            {
                assert(this->buffer.size() >= this->position);

                return this->buffer.size() - this->position;
            }

            inline void raw_put(const void* const data, const size_type size)
            {
                const byte_t* const array = reinterpret_cast<const byte_t*>(data);
                this->buffer.insert(this->buffer.end(), beginof(array), &array[size]);
            }

            inline void remove(const size_type size)
            {
                assert(this->raw_size() >= size);

                this->position += size;
            }

            template <typename T>
            inline T get(const size_type offset = size_type()) const
            {
                assert(this->raw_size() >= offset + sizeof(T));

                // Copy elision
                T t;
                std::memcpy(&t, this->raw_get() + offset, sizeof(t));
                return t;
            }

            template <typename T>
            inline void put(const T& t)
            {
                byte_t data[sizeof(t)];
                std::memcpy(&data, &t, sizeof(t));
                this->raw_put(data, sizeof(t));
            }

        private:
            byte_buffer(const byte_buffer&);
            byte_buffer& operator=(const byte_buffer&);
        };
    }
}
