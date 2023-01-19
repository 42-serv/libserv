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
            inline byte_buffer(size_type length) : buffer(length), position() {}

            inline byte_t* raw_buffer() throw()
            {
                return this->buffer.data();
            }

            inline size_type raw_length() const throw()
            {
                return this->buffer.size();
            }

            inline void raw_shrink(size_type length)
            {
                assert(length <= this->raw_length());

                this->buffer.resize(length);
            }

            inline const byte_t* get() const throw()
            {
                return &this->buffer[this->position];
            }

            template <typename T>
            inline T get(const size_type offset = size_type()) const
            {
                assert(this->size() >= offset + sizeof(T));

                // Copy elision
                T t;
                std::memcpy(&t, this->get() + offset, sizeof(t));
                return t;
            }

            inline size_type size() const throw()
            {
                assert(this->buffer.size() >= this->position);

                return this->buffer.size() - this->position;
            }

            inline void put(const void* const data, const size_type size)
            {
                const byte_t* const array = reinterpret_cast<const byte_t*>(data);
                this->buffer.insert(this->buffer.end(), beginof(array), &array[size]);
            }

            template <typename T>
            inline void put(const T& t)
            {
                byte_t data[sizeof(t)];
                std::memcpy(&data, &t, sizeof(t));
                this->put(data, sizeof(t));
            }

            inline void remove(const size_type size)
            {
                assert(this->size() >= size);

                this->position += size;
            }

        private:
            byte_buffer(const byte_buffer&);
            byte_buffer& operator=(const byte_buffer&);
        };
    }
}
