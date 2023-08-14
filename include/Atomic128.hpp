#ifndef __ATOMIC128_HPP__
#define __ATOMIC128_HPP__

#include <cstdint>
#include <type_traits>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace A128
{
    template <typename T>
    concept Size128 = requires(T a)
    {
        requires sizeof(a) == 16 && std::alignment_of_v<T> == 16;
    };

    template <Size128 T>
    class Atomic128
    {
        private:
            struct alignas(16) Data
            {
                uint64_t low;
                uint64_t high;
            };

            T value;

        public:
            Atomic128() : T()
            {
            }

            template <typename... Args>
            explicit Atomic128(Args&&... args) : value(std::forward<Args>(args)...)
            {
            }

            void Store(T desired)
            {
                Data* dest_data = (Data*)(&this->value);
                Data desired_data = *(Data*)&desired;

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
                __asm__ __volatile__
                (
                    "lock cmpxchg16b %[dest_data]"
                    : [dest_data] "+m" (*dest_data)
                    : "a" (dest_data->low),
                      "d" (dest_data->high),
                      "b" (desired_data.low),
                      "c" (desired_data.high)
                    : "memory",
                      "cc"
                );
#elif defined(_MSC_VER)
                _mm_storeu_si128(reinterpret_cast<__m128i*>(dest_data),
                                 _mm_loadu_si128(reinterpret_cast<const __m128i*>(&desired_data)));
#endif
            }

            T Load()
            {
                Data* src_data = (Data*)(&this->value);
                Data result_data = {0, 0};

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
                __asm__ __volatile__
                (
                    "lock cmpxchg16b %1"
                    : "+A" (result_data)
                    : "m" (*src_data),
                      "b" (0),
                      "c" (0)
                    : "cc"
                );
#elif defined(_MSC_VER)
                _mm_storeu_si128(reinterpret_cast<__m128i*>(&result_data),
                                 _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_data)));
#endif

                return *(T*)(&result_data);
            }

            T Exchange(T desired)
            {
                Data* dest_data = (Data*)(&this->value);
                Data desired_data = *(Data*)&desired;
                Data result_data;

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
                __asm__ __volatile__
                (
                    "lock cmpxchg16b %[dest_data]"
                    : [dest_data] "+m" (*dest_data),
                      [result_data] "=a" (result_data.low),
                      "=d" (result_data.high)
                    : "a" (dest_data->low),
                      "d" (dest_data->high),
                      "b" (desired_data.low),
                      "c" (desired_data.high)
                    : "memory",
                      "cc"
                );
#elif defined(_MSC_VER)
                _InterlockedExchange128(reinterpret_cast<int64_t*>(dest_data), desired_data.high, desired_data.low,
                                        reinterpret_cast<int64_t*>(&result_data));
#endif

                return *(T*)&result_data;
            }

            bool CompareExchange(T& expected, T desired)
            {
                Data* dest_data = (Data*)(&this->value);
                Data expected_data = *(Data*)&expected;
                Data desired_data = *(Data*)&desired;

                bool success;

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
                __asm__ __volatile__
                (
                    "lock cmpxchg16b %[dest_data]"
                    : [dest_data] "+m" (*dest_data),
                      [success] "=a" (success)
                    : "a" (expected_data.low),
                      "d" (expected_data.high),
                      "b" (desired_data.low),
                      "c" (desired_data.high)
                    : "memory",
                      "cc"
                );
#elif defined(_MSC_VER)
                success = _InterlockedCompareExchange128(reinterpret_cast<int64_t*>(dest_data), desired_data.high,
                                                         desired_data.low, reinterpret_cast<int64_t*>(&expected_data));
#endif

                expected = *(T*)&expected_data;
                return success;
            }
    };
}

#endif //__ATOMIC128_HPP__
