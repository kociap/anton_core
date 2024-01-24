#pragma once

#include <anton/type_traits.hpp>
#include <anton/types.hpp>

namespace anton {
    // Iterator category tags
    // Explicit constructors so that the tags may not be constructed via {}

    struct Input_Iterator_Tag {
        explicit Input_Iterator_Tag() = default;
    };

    struct Output_Iterator_Tag {
        explicit Output_Iterator_Tag() = default;
    };

    struct Forward_Iterator_Tag: public Input_Iterator_Tag {
        explicit Forward_Iterator_Tag() = default;
    };

    struct Bidirectional_Iterator_Tag: public Forward_Iterator_Tag {
        explicit Bidirectional_Iterator_Tag() = default;
    };

    struct Random_Access_Iterator_Tag: public Bidirectional_Iterator_Tag {
        explicit Random_Access_Iterator_Tag() = default;
    };

    // Contiguous_Iterator_Tag
    // Denotes an iterator whose elements are physically adjacent in memory.
    //
    struct Contiguous_Iterator_Tag: public Random_Access_Iterator_Tag {
        explicit Contiguous_Iterator_Tag() = default;
    };

    namespace detail {
        template<typename T, typename = void>
        struct Get_Pointer {
            using type = void;
        };

        template<typename T>
        struct Get_Pointer<T, void_sink<typename T::pointer>> {
            using type = typename T::pointer;
        };
    } // namespace detail

    // Iterator_Traits
    //
    template<typename T>
    struct Iterator_Traits {
        using difference_type = typename T::difference_type;
        using value_type = typename T::value_type;
        using pointer = typename detail::Get_Pointer<T>::type;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
    };

    template<typename T>
    struct Iterator_Traits<T*> {
        using difference_type = i64;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = Contiguous_Iterator_Tag;
    };

    template<typename T>
    struct Iterator_Traits<T const*> {
        using difference_type = i64;
        using value_type = T;
        using pointer = T const*;
        using reference = T const&;
        using iterator_category = Contiguous_Iterator_Tag;
    };

    // Is_Iterator_Category
    //
    template<typename T, typename Category>
    struct Is_Iterator_Category: public Is_Convertible<T, Category> {};

    template<typename T, typename Category>
    inline constexpr bool is_iterator_category = Is_Iterator_Category<T, Category>::value;

    // Is_Iterator_Wrapper
    // Checks whether a type is an iterator wrapper. Relies on the class daclaring wrapped_iterator_type alias.
    //
    template<typename T, typename = void>
    struct Is_Iterator_Wrapper: False_Type {};

    template<typename T>
    struct Is_Iterator_Wrapper<T, void_sink<typename T::wrapped_iterator_type>>: True_Type {};

    template<typename T>
    inline constexpr bool is_iterator_wrapper = Is_Iterator_Wrapper<T>::value;
} // namespace anton
