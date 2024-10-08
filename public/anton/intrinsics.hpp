#pragma once

#include <anton/types.hpp>

#define ANTON_UNUSED(x) ((void)(x))

namespace anton {
  [[noreturn]] void anton_assert(char8 const* message, char8 const* file,
                                 u64 line);
} // namespace anton

#define ANTON_DEBUG_BREAK()       \
  {                               \
    __asm__ __volatile__("int3"); \
  }

#if ANTON_COMPILER_CLANG
  #if ANTON_UNREACHABLE_ASSERTS
    #define ANTON_UNREACHABLE(msg)                    \
      ::anton::anton_assert(msg, __FILE__, __LINE__); \
      __builtin_unreachable()
  #else
    #define ANTON_UNREACHABLE(msg) __builtin_unreachable()
  #endif
  #define ANTON_UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define ANTON_LIKELY(x) __builtin_expect(!!(x), 1)
  #define ANTON_FORCEINLINE __attribute__((always_inline))
  #define ANTON_NOINLINE __attribute__((noinline))
#elif ANTON_COMPILER_GPP
  #if ANTON_UNREACHABLE_ASSERTS
    #define ANTON_UNREACHABLE(msg)                    \
      ::anton::anton_assert(msg, __FILE__, __LINE__); \
      __builtin_unreachable()
  #else
    #define ANTON_UNREACHABLE(msg) __builtin_unreachable()
  #endif
  #define ANTON_UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define ANTON_LIKELY(x) __builtin_expect(!!(x), 1)
  #define ANTON_FORCEINLINE __attribute__((always_inline))
  #define ANTON_NOINLINE __attribute__((noinline))
#elif ANTON_COMPILER_MSVC
  #if ANTON_UNREACHABLE_ASSERTS
    #define ANTON_UNREACHABLE(msg)                    \
      ::anton::anton_assert(msg, __FILE__, __LINE__); \
      __assume(0)
  #else
    #define ANTON_UNREACHABLE(msg) __assume(0)
  #endif
  #define ANTON_UNLIKELY(x) x
  #define ANTON_LIKELY(x) x
  #define ANTON_FORCEINLINE __forceinline
  #define ANTON_NOINLINE __declspec(noinline)
#else
  #if ANTON_UNREACHABLE_ASSERTS
    #define ANTON_UNREACHABLE(msg) \
      ::anton::anton_assert(msg, __FILE__, __LINE__)
  #else
    #define ANTON_UNREACHABLE(msg)
  #endif
  #define ANTON_UNLIKELY(x) x
  #define ANTON_LIKELY(x) x
  #define ANTON_FORCEINLINE
  #define ANTON_NOINLINE
#endif
