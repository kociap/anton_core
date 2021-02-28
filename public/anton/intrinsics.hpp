#pragma once

#include <anton/types.hpp>

#define ANTON_UNUSED(x) ((void)(x))

namespace anton {
    [[noreturn]] void anton_assert(char8 const* message, char8 const* file, u64 line);
} // namespace anton

#if defined(__clang__) || defined(__GNUC__)
#    if ANTON_UNREACHABLE_ASSERTS
#        define ANTON_UNREACHABLE() ::anton::anton_assert(u8"unreachable", __FILE__, __LINE__)
#    else
#        define ANTON_UNREACHABLE() __builtin_unreachable()
#    endif
#    define ANTON_UNLIKELY(x) __builtin_expect(!!(x), 0)
#    define ANTON_LIKELY(x) __builtin_expect(!!(x), 1)
#    define ANTON_FORCEINLINE __attribute__((always_inline))
#    define ANTON_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#    if ANTON_UNREACHABLE_ASSERTS
#        define ANTON_UNREACHABLE() ::anton::anton_assert(u8"unreachable", __FILE__, __LINE__)
#    else
#        define ANTON_UNREACHABLE() __assume(0)
#    endif
#    define ANTON_UNLIKELY(x) x
#    define ANTON_LIKELY(x) x
#    define ANTON_FORCEINLINE __forceinline
#    define ANTON_NOINLINE __declspec(noinline)
#else
#    if ANTON_UNREACHABLE_ASSERTS
#        define ANTON_UNREACHABLE() ::anton::anton_assert(u8"unreachable", __FILE__, __LINE__)
#    else
#        define ANTON_UNREACHABLE()
#    endif
#    define ANTON_UNLIKELY(x) x
#    define ANTON_LIKELY(x) x
#    define ANTON_FORCEININLINE
#    define ANTON_NOINLINE
#endif
