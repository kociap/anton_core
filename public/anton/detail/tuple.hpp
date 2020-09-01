#pragma once

#include <anton/types.hpp>

namespace anton {
    template<typename>
    struct Tuple_Size;

    template<typename T>
    constexpr u64 tuple_size = Tuple_Size<T>::value;

    template<usize, typename>
    struct Tuple_Element;

    template<usize I, typename T>
    using tuple_element = typename Tuple_Element<I, T>::type;
} // namespace anton

// We provide std::tuple_size and std::tuple_element to enable structured bindings on certain types
// so we forward declare those types so that we do not have to deal with errors or std headers.
namespace std {
    template<typename>
    struct tuple_size;

    template<anton::usize, typename>
    struct tuple_element;
} // namespace std
