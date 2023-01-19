/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

namespace ft
{
    namespace serv
    {
        class task_base
        {
        public:
            task_base();
            virtual ~task_base();

            virtual void run() throw() = 0;

        private:
            task_base(const task_base&);
            task_base& operator=(const task_base&);
        };
    }
}

#define FT_SERV_DEFINE_TASK(name, expr)     \
    class name : public ft::serv::task_base \
    {                                       \
    public:                                 \
        inline void run() throw()           \
        {                                   \
            expr;                           \
        }                                   \
    }

#define FT_SERV_DEFINE_TASK_1(name, T1, a1, expr) \
    class name : public ft::serv::task_base       \
    {                                             \
    private:                                      \
        T1 a1;                                    \
                                                  \
    public:                                       \
        name(const T1& a1)                        \
            : a1(a1)                              \
        {                                         \
        }                                         \
                                                  \
        inline void run() throw()                 \
        {                                         \
            expr;                                 \
        }                                         \
    }

#define FT_SERV_DEFINE_TASK_2(name, T1, a1, T2, a2, expr) \
    class name : public ft::serv::task_base               \
    {                                                     \
    private:                                              \
        T1 a1;                                            \
        T2 a2;                                            \
                                                          \
    public:                                               \
        name(const T1& a1, const T2& a2)                  \
            : a1(a1), a2(a2)                              \
        {                                                 \
        }                                                 \
                                                          \
        inline void run() throw()                         \
        {                                                 \
            expr;                                         \
        }                                                 \
    }

#define FT_SERV_DEFINE_TASK_3(name, T1, a1, T2, a2, T3, a3, expr) \
    class name : public ft::serv::task_base                       \
    {                                                             \
    private:                                                      \
        T1 a1;                                                    \
        T2 a2;                                                    \
        T3 a3;                                                    \
                                                                  \
    public:                                                       \
        name(const T1& a1, const T2& a2, const T3& a3)            \
            : a1(a1), a2(a2), a3(a3)                              \
        {                                                         \
        }                                                         \
                                                                  \
        inline void run() throw()                                 \
        {                                                         \
            expr;                                                 \
        }                                                         \
    }

#define FT_SERV_DEFINE_TASK_4(name, T1, a1, T2, a2, T3, a3, T4, a4, expr) \
    class name : public ft::serv::task_base                               \
    {                                                                     \
    private:                                                              \
        T1 a1;                                                            \
        T2 a2;                                                            \
        T3 a3;                                                            \
        T4 a4;                                                            \
                                                                          \
    public:                                                               \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4)      \
            : a1(a1), a2(a2), a3(a3), a4(a4)                              \
        {                                                                 \
        }                                                                 \
                                                                          \
        inline void run() throw()                                         \
        {                                                                 \
            expr;                                                         \
        }                                                                 \
    }

#define FT_SERV_DEFINE_TASK_5(name, T1, a1, T2, a2, T3, a3, T4, a4, T5, a5, expr)  \
    class name : public ft::serv::task_base                                        \
    {                                                                              \
    private:                                                                       \
        T1 a1;                                                                     \
        T2 a2;                                                                     \
        T3 a3;                                                                     \
        T4 a4;                                                                     \
        T5 a5;                                                                     \
                                                                                   \
    public:                                                                        \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5) \
            : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5)                               \
        {                                                                          \
        }                                                                          \
                                                                                   \
        inline void run() throw()                                                  \
        {                                                                          \
            expr;                                                                  \
        }                                                                          \
    }

#define FT_SERV_DEFINE_TASK_6(name, T1, a1, T2, a2, T3, a3, T4, a4, T5, a5, T6, a6, expr)        \
    class name : public ft::serv::task_base                                                      \
    {                                                                                            \
    private:                                                                                     \
        T1 a1;                                                                                   \
        T2 a2;                                                                                   \
        T3 a3;                                                                                   \
        T4 a4;                                                                                   \
        T5 a5;                                                                                   \
        T6 a6;                                                                                   \
                                                                                                 \
    public:                                                                                      \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6) \
            : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6)                                     \
        {                                                                                        \
        }                                                                                        \
                                                                                                 \
        inline void run() throw()                                                                \
        {                                                                                        \
            expr;                                                                                \
        }                                                                                        \
    }

#define FT_SERV_DEFINE_TASK_7(name, T1, a1, T2, a2, T3, a3, T4, a4, T5, a5, T6, a6, T7, a7, expr)              \
    class name : public ft::serv::task_base                                                                    \
    {                                                                                                          \
    private:                                                                                                   \
        T1 a1;                                                                                                 \
        T2 a2;                                                                                                 \
        T3 a3;                                                                                                 \
        T4 a4;                                                                                                 \
        T5 a5;                                                                                                 \
        T6 a6;                                                                                                 \
        T7 a7;                                                                                                 \
                                                                                                               \
    public:                                                                                                    \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7) \
            : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7)                                           \
        {                                                                                                      \
        }                                                                                                      \
                                                                                                               \
        inline void run() throw()                                                                              \
        {                                                                                                      \
            expr;                                                                                              \
        }                                                                                                      \
    }

#define FT_SERV_DEFINE_TASK_8(name, T1, a1, T2, a2, T3, a3, T4, a4, T5, a5, T6, a6, T7, a7, T8, a8, expr)                    \
    class name : public ft::serv::task_base                                                                                  \
    {                                                                                                                        \
    private:                                                                                                                 \
        T1 a1;                                                                                                               \
        T2 a2;                                                                                                               \
        T3 a3;                                                                                                               \
        T4 a4;                                                                                                               \
        T5 a5;                                                                                                               \
        T6 a6;                                                                                                               \
        T7 a7;                                                                                                               \
        T8 a8;                                                                                                               \
                                                                                                                             \
    public:                                                                                                                  \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8) \
            : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7), a8(a8)                                                 \
        {                                                                                                                    \
        }                                                                                                                    \
                                                                                                                             \
        inline void run() throw()                                                                                            \
        {                                                                                                                    \
            expr;                                                                                                            \
        }                                                                                                                    \
    }

#define FT_SERV_DEFINE_TASK_9(name, T1, a1, T2, a2, T3, a3, T4, a4, T5, a5, T6, a6, T7, a7, T8, a8, T9, a9, expr)                          \
    class name : public ft::serv::task_base                                                                                                \
    {                                                                                                                                      \
    private:                                                                                                                               \
        T1 a1;                                                                                                                             \
        T2 a2;                                                                                                                             \
        T3 a3;                                                                                                                             \
        T4 a4;                                                                                                                             \
        T5 a5;                                                                                                                             \
        T6 a6;                                                                                                                             \
        T7 a7;                                                                                                                             \
        T8 a8;                                                                                                                             \
        T9 a9;                                                                                                                             \
                                                                                                                                           \
    public:                                                                                                                                \
        name(const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9) \
            : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7), a8(a8), a9(a9)                                                       \
        {                                                                                                                                  \
        }                                                                                                                                  \
                                                                                                                                           \
        inline void run() throw()                                                                                                          \
        {                                                                                                                                  \
            expr;                                                                                                                          \
        }                                                                                                                                  \
    }
