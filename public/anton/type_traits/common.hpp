#pragma once

#include <anton/type_traits/base.hpp>
#include <anton/types.hpp>

namespace anton::detail {
  // Is_Array
  //
  template<typename T>
  struct Is_Array: False_Type {};

  template<typename T>
  struct Is_Array<T[]>: True_Type {};

  template<typename T, usize N>
  struct Is_Array<T[N]>: True_Type {};

  // Is_Same
  //
  template<typename T1, typename T2>
  struct Is_Same: False_Type {};

  template<typename T>
  struct Is_Same<T, T>: True_Type {};

  template<typename T1, typename T2>
  constexpr bool is_same = Is_Same<T1, T2>::value;

  // Is_Same_Type
  // Compares 'type' member typedefs.
  //
  template<typename T1, typename T2>
  struct Is_Same_Type {
    static constexpr bool value = is_same<typename T1::type, typename T2::type>;
  };
} // namespace anton::detail
