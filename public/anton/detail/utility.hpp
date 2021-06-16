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

    // Make_Integer_Sequence
    // Creates an Integer_Sequence which starts at 0 and ends at N - 1.
    //
    template<typename T, T N>
    struct Make_Integer_Sequence {
        using type = __make_integer_seq<Integer_Sequence, T, N>;
    };

    template<typename T, T N>
    using make_integer_sequence = __make_integer_seq<Integer_Sequence, T, N>;
} // namespace anton
