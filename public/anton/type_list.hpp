#pragma once

#include <anton/detail/tuple.hpp>
#include <anton/type_traits/base.hpp>

namespace anton {
  template<typename... Ts>
  struct Type_List {
  public:
    using type = Type_List<Ts...>;
  };

  template<typename... Types>
  struct Tuple_Size<Type_List<Types...>>
    : Integral_Constant<u64, sizeof...(Types)> {};

  template<typename... Types>
  struct Tuple_Size<Type_List<Types...> const>
    : Integral_Constant<u64, sizeof...(Types)> {};

  namespace detail {
    template<u64 N, u64 X, typename>
    struct Tuple_Element;

    template<u64 N, u64 X, typename T, typename... Types>
    struct Tuple_Element<N, X, Type_List<T, Types...>> {
      using type = typename Tuple_Element<N, X + 1, Type_List<Types...>>::type;
    };

    template<u64 N, typename T, typename... Types>
    struct Tuple_Element<N, N, Type_List<T, Types...>> {
      using type = T;
    };
  } // namespace detail

  template<u64 Index, typename... Types>
  struct Tuple_Element<Index, Type_List<Types...>> {
    using type =
      typename detail::Tuple_Element<Index, 0, Type_List<Types...>>::type;
  };

  template<u64 Index, typename... Types>
  struct Tuple_Element<Index, Type_List<Types...> const> {
    using type =
      typename detail::Tuple_Element<Index, 0, Type_List<Types...>>::type const;
  };
} // namespace anton
