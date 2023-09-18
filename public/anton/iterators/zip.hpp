#pragma once

#include <anton/iterators/base.hpp>
#include <anton/tuple.hpp>

namespace anton {
    namespace detail {
        template<typename... Ts, u64... Indices>
        void zip_increment(Tuple<Ts...>& tuple, integer_sequence<u64, Indices...>) {
            (++static_cast<Tuple_Child<Indices, Ts>&>(tuple).value, ...);
        }

        template<typename... Ts, u64... Indices>
        void zip_decrement(Tuple<Ts...>& tuple, integer_sequence<u64, Indices...>) {
            (--static_cast<Tuple_Child<Indices, Ts>&>(tuple).value, ...);
        }

        template<typename Return, typename... Ts, u64... Indices>
        [[nodiscard]] Return zip_dereference(Tuple<Ts...> const& tuple, integer_sequence<u64, Indices...>) {
            return Return((*static_cast<Tuple_Child<Indices, Ts> const&>(tuple).value)...);
        }

        template<typename... Ts, u64... Indices>
        [[nodiscard]] bool zip_equal(Tuple<Ts...> const& lhs, Tuple<Ts...> const& rhs, integer_sequence<u64, Indices...>) {
            return ((static_cast<Tuple_Child<Indices, Ts> const&>(lhs).value == static_cast<Tuple_Child<Indices, Ts> const&>(rhs).value) || ...);
        }
    } // namespace detail

    // TODO: implement iterator difference
    template<typename... Iterators>
    struct Zip_Iterator {
        static_assert(sizeof...(Iterators) > 1, "Zip_Iterator > 1");

        using value_type = Tuple<typename Iterator_Traits<Iterators>::value_type...>;
        using pointer = Tuple<typename Iterator_Traits<Iterators>::pointer...>;
        using reference = Tuple<typename Iterator_Traits<Iterators>::reference...>;
        using difference_type = i64;
        using iterator_category = Random_Access_Iterator_Tag;
        using wrapped_iterator_type = Tuple<Iterators...>;

        Zip_Iterator() = default;
        explicit Zip_Iterator(Iterators const&... iterators): _iterators(iterators...) {}
        explicit Zip_Iterator(Iterators&&... iterators): _iterators(ANTON_MOV(iterators)...) {}

        // TODO: Implement the below functions with a flat function call instead of apply. See zip_dereference.

        [[nodiscard]] Tuple<Iterators const&...> base() const {
            return apply(_iterators, [](auto&&... iterators) -> Tuple<Iterators const&...> { return {iterators...}; });
        }

        Zip_Iterator& operator++() {
            detail::zip_increment(_iterators, make_integer_sequence<u64, sizeof...(Iterators)>());
            return *this;
        }

        Zip_Iterator& operator--() {
            detail::zip_decrement(_iterators, make_integer_sequence<u64, sizeof...(Iterators)>());
            return *this;
        }

        Zip_Iterator operator++(int) {
            Zip_Iterator copy = *this;
            ++(*this);
            return copy;
        }

        Zip_Iterator operator--(int) {
            Zip_Iterator copy = *this;
            --(*this);
            return copy;
        }

        Zip_Iterator& operator+=(difference_type n) {
            apply(_iterators, [n](auto&&... iterators) -> void { ((iterators += n), ...); });
            return *this;
        }

        Zip_Iterator& operator-=(difference_type n) {
            apply(_iterators, [n](auto&&... iterators) -> void { ((iterators -= n), ...); });
            return *this;
        }

        [[nodiscard]] Zip_Iterator operator+(difference_type n) const {
            Zip_Iterator iterator(*this);
            iterator += n;
            return iterator;
        }

        [[nodiscard]] friend Zip_Iterator operator+(difference_type n, Zip_Iterator const& a) {
            Zip_Iterator iterator(a);
            iterator += n;
            return iterator;
        }

        [[nodiscard]] Zip_Iterator operator-(difference_type n) const {
            Zip_Iterator iterator(*this);
            iterator -= n;
            return iterator;
        }

        [[nodiscard]] difference_type operator-(Zip_Iterator const& other) {
            return get<0>(_iterators) - get<0>(other._iterators);
        }

        [[nodiscard]] ANTON_FORCEINLINE reference operator*() const {
            return detail::zip_dereference<reference>(_iterators, make_integer_sequence<u64, sizeof...(Iterators)>());
        }

        [[nodiscard]] reference operator[](difference_type n) const {
            return apply(_iterators, [n](auto&&... iterators) -> reference { return {*(iterators + n)...}; });
        }

        // operator==
        // Compare the contained iterators for equality using the rule 'if any'.
        //
        // Returns:
        // true if any of the contained iterators compare equal.
        //
        [[nodiscard]] ANTON_FORCEINLINE bool operator==(Zip_Iterator const& rhs) const {
            return detail::zip_equal(_iterators, rhs._iterators, make_integer_sequence<u64, sizeof...(Iterators)>());
        }

        // operator!=
        // Compare the contained iterators for inequality using the rule 'if all'.
        // Negation of the operator==.
        //
        // Returns:
        // false if any of the contained iterators compare equal.
        //
        [[nodiscard]] ANTON_FORCEINLINE bool operator!=(Zip_Iterator const& rhs) const {
            return !detail::zip_equal(_iterators, rhs._iterators, make_integer_sequence<u64, sizeof...(Iterators)>());
        }

        // TODO: Implement.

        // [[nodiscard]] bool operator<(Zip_Iterator const& rhs) const {
        //     return detail::zip_tuple_less(_iterators, rhs._iterators);
        // }

        // [[nodiscard]] bool operator>(Zip_Iterator const& rhs) const {
        //     return detail::zip_tuple_less(rhs._iterators, _iterators);
        // }

        // [[nodiscard]] bool operator<=(Zip_Iterator const& rhs) const {
        //     return !(rhs > *this);
        // }

        // [[nodiscard]] bool operator>=(Zip_Iterator const& rhs) const {
        //     return !(rhs < *this);
        // }

    private:
        Tuple<Iterators...> _iterators;
    };

    template<typename... T>
    Zip_Iterator(T const&...) -> Zip_Iterator<T...>;
    template<typename... T>
    Zip_Iterator(T&&...) -> Zip_Iterator<T...>;
} // namespace anton
