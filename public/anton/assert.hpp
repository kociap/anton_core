#pragma once

#include <anton/intrinsics.hpp>
#include <anton/types.hpp>

namespace anton {
    [[noreturn]] void anton_assert(char8 const* message, char8 const* file, u64 line);
} // namespace anton

#define ANTON_FAIL(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton::anton_assert(msg, __FILE__, __LINE__))

#if ANTON_DEBUG
#    define ANTON_ASSERT(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton::anton_assert(msg, __FILE__, __LINE__))
#else
#    define ANTON_ASSERT(condition, msg) ((void)0)
#endif

#if !ANTON_DISABLE_VERIFY
// ANTON_VERIFY
// Debug and Release builds assert. Must be disabled explicitly.
#    define ANTON_VERIFY(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton::anton_assert(msg, __FILE__, __LINE__))
#else
#    define ANTON_VERIFY() ((void)0)
#endif // !ANTON_DISABLE_VERIFY
