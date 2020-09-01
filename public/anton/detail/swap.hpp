#pragma once

#include <anton/detail/traits_base.hpp>
#include <anton/detail/traits_utility.hpp>

namespace anton {
    template<typename T>
    constexpr void swap(T& a, T& b) {
        T tmp(move(a));
        a = move(b);
        b = move(tmp);
    }

    template<typename T, typename U, typename = void>
    struct Is_Swappable_With: False_Type {};

    // Note: swap left unprefixed to allow ADL of user-defined swap specializations
    template<typename T, typename U>
    struct Is_Swappable_With<T, U, void_sink<decltype(swap(declval<U&>(), declval<T&>())), decltype(swap(declval<T&>(), declval<U&>()))>>: True_Type {};

    template<typename T, typename U>
    constexpr bool is_swappable_with = Is_Swappable_With<T, U>::value;

    template<typename T>
    struct Is_Swappable: Is_Swappable_With<T, T> {};

    template<typename T>
    constexpr bool is_swappable = Is_Swappable<T>::value;
} // namespace anton
