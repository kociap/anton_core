#pragma once

#include <anton/detail/utility_common.hpp>
#include <anton/tags.hpp>
#include <anton/types.hpp>

namespace anton {
    namespace detail {
        template<u64 Index, typename Type>
        class Tuple_Child {
        public:
            constexpr Tuple_Child() = default;

            template<typename T>
            constexpr Tuple_Child(T&& arg): _element(forward<T>(arg)) {}

            Type _element;
        };

        template<typename, typename...>
        class Tuple_Expand;

        template<u64... Indices, typename... Types>
        class Tuple_Expand<Integer_Sequence<u64, Indices...>, Types...>: public Tuple_Child<Indices, Types>... {
        public:
            constexpr Tuple_Expand() = default;

            template<typename... Args>
            constexpr Tuple_Expand(Variadic_Construct_Tag, Args&&... args): Tuple_Child<Indices, Types>(forward<Args>(args))... {}
        };
    } // namespace detail

    template<typename... Ts>
    class Tuple: public detail::Tuple_Expand<make_integer_sequence<u64, sizeof...(Ts)>, Ts...> {
    private:
        using base_t = detail::Tuple_Expand<make_integer_sequence<u64, sizeof...(Ts)>, Ts...>;

    public:
        constexpr Tuple() = default;

        // TODO: Add conditional explicit
        template<typename... Args, enable_if<sizeof...(Args) == sizeof...(Ts) && (sizeof...(Args) > 0), int> = 0>
        constexpr Tuple(Args&&... args): base_t(variadic_construct, forward<Args>(args)...) {}
    };

    template<typename... Types>
    struct Tuple_Size<Tuple<Types...>>: Integral_Constant<u64, sizeof...(Types)> {};

    template<typename... Types>
    struct Tuple_Size<Tuple<Types...> const>: Integral_Constant<u64, sizeof...(Types)> {};

    namespace detail {
        template<u64 N, u64 X, typename>
        struct Tuple_Element;

        template<u64 N, u64 X, typename T, typename... Types>
        struct Tuple_Element<N, X, Tuple<T, Types...>> {
            using type = typename Tuple_Element<N, X + 1, Tuple<Types...>>::type;
        };

        template<u64 N, typename T, typename... Types>
        struct Tuple_Element<N, N, Tuple<T, Types...>> {
            using type = T;
        };
    } // namespace detail

    template<u64 Index, typename... Types>
    struct Tuple_Element<Index, Tuple<Types...>> {
        using type = typename detail::Tuple_Element<Index, 0, Tuple<Types...>>::type;
    };

    template<u64 Index, typename... Types>
    struct Tuple_Element<Index, Tuple<Types...> const> {
        using type = add_const<typename detail::Tuple_Element<Index, 0, Tuple<Types...>>::type>;
    };

    namespace detail {
        template<typename T, u64 Index>
        constexpr T& get_helper_type(Tuple_Child<Index, T>& element) {
            return static_cast<T&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T const& get_helper_type(Tuple_Child<Index, T> const& element) {
            return static_cast<T const&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T&& get_helper_type(Tuple_Child<Index, T>&& element) {
            return static_cast<T&&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T const&& get_helper_type(Tuple_Child<Index, T> const&& element) {
            return static_cast<T const&&>(element._element);
        }
    } // namespace detail

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>>& get(Tuple<Types...>& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type&>(static_cast<detail::Tuple_Child<Index, type>&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>> const& get(Tuple<Types...> const& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type const&>(static_cast<detail::Tuple_Child<Index, type> const&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>>&& get(Tuple<Types...>&& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type&&>(static_cast<detail::Tuple_Child<Index, type>&&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>> const&& get(Tuple<Types...> const&& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type const&&>(static_cast<detail::Tuple_Child<Index, type> const&&>(t)._element);
    }

    template<typename T, typename... Types>
    constexpr T& get(Tuple<Types...>& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T const& get(Tuple<Types...> const& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T&& get(Tuple<Types...>&& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T const&& get(Tuple<Types...> const&& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename... Types>
    constexpr Tuple<decay<Types>...> make_tuple(Types&&... args) {
        return Tuple<decay<Types>...>(forward<Types>(args)...);
    }

    namespace detail {
        template<typename Callable, typename Tuple, u64... Indices>
        constexpr decltype(auto) apply(Callable&& callable, Tuple&& tuple, integer_sequence<u64, Indices...>) {
            return callable(get<Indices>(forward<Tuple>(tuple))...);
        }
    } // namespace detail

    template<typename Callable, typename Tuple>
    constexpr decltype(auto) apply(Callable&& callable, Tuple&& tuple) {
        return detail::apply(forward<Callable>(callable), forward<Tuple>(tuple), make_integer_sequence<u64, tuple_size<remove_reference<Tuple>>>());
    }
} // namespace anton

// We provide std::tuple_size and std::tuple_element to enable structured bindings
namespace std {
    template<typename... Types>
    struct tuple_size<anton::Tuple<Types...>>: anton::Tuple_Size<anton::Tuple<Types...>> {};

    template<typename... Types>
    struct tuple_size<anton::Tuple<Types...> const>: anton::Tuple_Size<anton::Tuple<Types...> const> {};

    template<anton::usize I, typename... Types>
    struct tuple_element<I, anton::Tuple<Types...>>: anton::Tuple_Element<I, anton::Tuple<Types...>> {};

    template<anton::usize I, typename... Types>
    struct tuple_element<I, anton::Tuple<Types...> const>: anton::Tuple_Element<I, anton::Tuple<Types...> const> {};
} // namespace std
