#pragma once

#include <anton/detail/utility_common.hpp>
#include <anton/types.hpp>

namespace anton {
    template<typename... Ts>
    class Type_List {
    public:
        using type = Type_List<Ts...>;
    };

    template<typename... Types>
    struct Tuple_Size<Type_List<Types...>>: Integral_Constant<u64, sizeof...(Types)> {};

    template<typename... Types>
    struct Tuple_Size<Type_List<Types...> const>: Integral_Constant<u64, sizeof...(Types)> {};

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
        using type = typename detail::Tuple_Element<Index, 0, Type_List<Types...>>::type;
    };

    template<u64 Index, typename... Types>
    struct Tuple_Element<Index, Type_List<Types...> const> {
        using type = add_const<typename detail::Tuple_Element<Index, 0, Type_List<Types...>>::type>;
    };

    // Type_Selection_Sort
    // Sorts elements of a Type_List according to Compare using selection sort.
    // Compare must provide a templated on 2 parameters static member variable called compare
    // that can be converted to bool.
    //
    template<typename List, typename Compare>
    struct Type_Selection_Sort {
    private:
        template<typename TS_List, u64 TS_I, u64 TS_J>
        struct Type_Swap {
        private:
            template<typename _TS_List, u64 _TS_I, u64 _TS_J, typename>
            struct _Type_Swap;

            template<typename _TS_List, u64 _TS_I, u64 _TS_J, u64... Indices>
            struct _Type_Swap<_TS_List, _TS_I, _TS_J, Integer_Sequence<u64, Indices...>> {
                using type = Type_List<tuple_element<(Indices != _TS_I && Indices != _TS_J) ? Indices : (Indices == _TS_I ? _TS_J : _TS_I), _TS_List>...>;
            };

        public:
            using type = typename _Type_Swap<TS_List, TS_I, TS_J, make_integer_sequence<u64, tuple_size<List>>>::type;
        };

        template<u64 I, u64 J, u64 Least, u64 List_Size, typename TS_List, typename TS_Compare>
        struct Inner_Loop {
        private:
            static constexpr bool compare_res = TS_Compare::template compare<tuple_element<Least, TS_List>, tuple_element<J, TS_List>>;

        public:
            using type = typename Inner_Loop<I, J + 1, compare_res ? Least : J, List_Size, TS_List, TS_Compare>::type;
        };

        template<u64 I, u64 Least, u64 List_Size, typename TS_List, typename TS_Compare>
        struct Inner_Loop<I, List_Size, Least, List_Size, TS_List, TS_Compare> {
            using type = typename Type_Swap<List, I, Least>::type;
        };

        template<u64 I, u64 List_Size, typename TS_List, typename TS_Compare>
        struct Outer_Loop {
            using iteration_res = typename Inner_Loop<I, I + 1, I, List_Size, TS_List, TS_Compare>::type;
            using type = typename Outer_Loop<I + 1, List_Size, iteration_res, TS_Compare>::type;
        };

        template<u64 List_Size, typename TS_List, typename TS_Compare>
        struct Outer_Loop<List_Size, List_Size, TS_List, TS_Compare> {
            using type = List;
        };

    public:
        using type = typename Outer_Loop<0, tuple_size<List>, List, Compare>::type;
    };

    template<typename List, typename Compare>
    using type_selection_sort = typename Type_Selection_Sort<List, Compare>::type;
} // namespace anton
