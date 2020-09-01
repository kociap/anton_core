#pragma once

namespace anton {
    // Integer_Sequence
    //
    template<typename T, T... Integers>
    struct Integer_Sequence {
        using type = Integer_Sequence<T, Integers...>;
    };

    template<typename T, T... Integers>
    using integer_sequence = Integer_Sequence<T, Integers...>;

    namespace detail {
        template<typename T, T, T, typename>
        struct Make_Integer_Sequence;

        template<typename T, T N, T X, T... Integers>
        struct Make_Integer_Sequence<T, N, X, Integer_Sequence<T, Integers...>> {
            using type = typename Make_Integer_Sequence<T, N, X + 1, Integer_Sequence<T, Integers..., X>>::type;
        };

        template<typename T, T N, T... Integers>
        struct Make_Integer_Sequence<T, N, N, Integer_Sequence<T, Integers...>> {
            using type = Integer_Sequence<T, Integers...>;
        };
    } // namespace detail

    // Make_Integer_Sequence
    // Creates an Integer_Sequence which starts at 0 and ends at N - 1.
    //
    template<typename T, T N>
    struct Make_Integer_Sequence {
        using type = typename detail::Make_Integer_Sequence<T, N, 0, Integer_Sequence<T>>::type;
    };

    template<typename T, T N>
    using make_integer_sequence = typename Make_Integer_Sequence<T, N>::type;
} // namespace anton
