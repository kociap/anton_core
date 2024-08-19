#pragma once

namespace anton {
// Clang and MSVC provide __make_integer_seq to efficiently implement
// make_integer_sequence, but GNU g++ instead defines __integer_pack which
// expands to a pack of integers from 0 to N-1
#if ANTON_COMPILER_GPP
  #define ANTON_MAKE_INTEGER_SEQUENCE(SEQ, T, N) SEQ<T, __integer_pack(N)...>
#else
  #define ANTON_MAKE_INTEGER_SEQUENCE(SEQ, T, N) __make_integer_seq<SEQ, T, N>
#endif

  // Integer_Sequence
  //
  template<typename T, T... Integers>
  struct Integer_Sequence {
    using type = Integer_Sequence<T, Integers...>;
  };

  template<typename T, T... Integers>
  using integer_sequence = Integer_Sequence<T, Integers...>;

  // Make_Integer_Sequence Creates an Integer_Sequence which starts at 0 and
  // ends at N-1.
  //
  template<typename T, T N>
  struct Make_Integer_Sequence {
    using type = ANTON_MAKE_INTEGER_SEQUENCE(Integer_Sequence, T, N);
  };

  template<typename T, T N>
  using make_integer_sequence = ANTON_MAKE_INTEGER_SEQUENCE(Integer_Sequence, T,
                                                            N);

#undef ANTON_MAKE_INTEGER_SEQUENCE
} // namespace anton
