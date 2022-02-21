#pragma once

#include <anton/iterators/base.hpp>
#include <anton/tuple.hpp>

namespace anton {
    namespace detail {
        template<typename Tuple, u64... Indices>
        [[nodiscard]] bool zip_tuple_equal_impl(Tuple const& lhs, Tuple const& rhs, integer_sequence<u64, Indices...>) {
            return ((get<Indices>(lhs) == get<Indices>(rhs)) && ...);
        }

        template<typename... Types>
        [[nodiscard]] bool zip_tuple_equal(Tuple<Types...> const& lhs, Tuple<Types...> const& rhs) {
            return zip_tuple_equal_impl(lhs, rhs, make_integer_sequence<u64, sizeof...(Types)>());
        }

        template<typename Tuple, u64... Indices>
        [[nodiscard]] bool zip_tuple_less_impl(Tuple const& lhs, Tuple const& rhs, integer_sequence<u64, Indices...>) {
            return ((get<Indices>(lhs) < get<Indices>(rhs)) && ...);
        }

        template<typename... Types>
        [[nodiscard]] bool zip_tuple_less(Tuple<Types...> const& lhs, Tuple<Types...> const& rhs) {
            return zip_tuple_less_impl(lhs, rhs, make_integer_sequence<u64, sizeof...(Types)>());
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

        Tuple<Iterators const&...> base() const {
            return apply(_iterators, [](auto&&... iterators) -> Tuple<Iterators const&...> { return {iterators...}; });
        }

        Zip_Iterator& operator++() {
            apply(_iterators, [](auto&&... iterators) -> void { (++iterators, ...); });
            return *this;
        }

        Zip_Iterator& operator--() {
            apply(_iterators, [](auto&&... iterators) -> void { (--iterators, ...); });
            return *this;
        }

        Zip_Iterator operator++(int) {
            Zip_Iterator copy = *this;
            apply(_iterators, [](auto&&... iterators) -> void { (++iterators, ...); });
            return copy;
        }

        Zip_Iterator operator--(int) {
            Zip_Iterator copy = *this;
            apply(_iterators, [](auto&&... iterators) -> void { (--iterators, ...); });
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

        [[nodiscard]] reference operator*() const {
            return apply(_iterators, [](auto&&... iterators) -> reference { return {*iterators...}; });
        }

        [[nodiscard]] reference operator[](difference_type n) const {
            return apply(_iterators, [](auto&&... iterators) -> reference { return {*(iterators + n)}; });
        }

        [[nodiscard]] bool operator==(Zip_Iterator const& rhs) const {
            return detail::zip_tuple_equal(_iterators, rhs._iterators);
        }

        [[nodiscard]] bool operator!=(Zip_Iterator const& rhs) const {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator<(Zip_Iterator const& rhs) const {
            return detail::zip_tuple_less(_iterators, rhs._iterators);
        }

        [[nodiscard]] bool operator>(Zip_Iterator const& rhs) const {
            return detail::zip_tuple_less(rhs._iterators, _iterators);
        }

        [[nodiscard]] bool operator<=(Zip_Iterator const& rhs) const {
            return !(rhs > *this);
        }

        [[nodiscard]] bool operator>=(Zip_Iterator const& rhs) const {
            return !(rhs < *this);
        }

    private:
        Tuple<Iterators...> _iterators;
    };

    template<typename... T>
    Zip_Iterator(T const&...) -> Zip_Iterator<T...>;
    template<typename... T>
    Zip_Iterator(T&&...) -> Zip_Iterator<T...>;
} // namespace anton
