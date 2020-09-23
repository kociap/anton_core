#pragma once

#include <anton/detail/traits_transformations.hpp>

#define ANTON_MOV(x) (static_cast<anton::remove_reference<decltype(x)>&&>(x))
#define ANTON_FWD(x) (static_cast<decltype(x)&&>(x))

namespace anton {
    template<typename T>
    [[nodiscard]] constexpr T&& forward(remove_reference<T>& v) {
        return static_cast<T&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr T&& forward(remove_reference<T>&& v) {
        // We don't really care about safety, so we just throw it out the window
        // static_assert(!is_lvalue_reference<T>, "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr remove_reference<T>&& move(T&& v) {
        return static_cast<remove_reference<T>&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr add_const<T>& as_const(T& v) {
        return v;
    }

    template<typename T>
    void as_const(T const&&) = delete;
} // namespace anton
