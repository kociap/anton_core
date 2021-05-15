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

    // Iterator_Traits
    //
    template<typename T>
    struct Iterator_Traits {
        using difference_type = typename T::difference_type;
        using value_type = typename T::value_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
    };

    template<typename T>
    struct Iterator_Traits<T*> {
        using difference_type = isize;
        using value_type = remove_const<T>;
        using pointer = T*;
        using reference = T&;
        using iterator_category = Contiguous_Iterator_Tag;
    };

    template<typename Iterator>
    struct Reverse_Iterator {
    public:
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using difference_type = typename Iterator::difference_type;
        using iterator_category = typename Iterator::iterator_category;
        using wrapped_iterator_type = Iterator;

        explicit Reverse_Iterator(wrapped_iterator_type const& iter): _iterator(iter) {}
        explicit Reverse_Iterator(wrapped_iterator_type&& iter): _iterator(ANTON_MOV(iter)) {}

        wrapped_iterator_type const& base() const {
            return _iterator;
        }

        Reverse_Iterator& operator++() {
            --_iterator;
            return *this;
        }

        Reverse_Iterator& operator--() {
            ++_iterator;
            return *this;
        }

        Reverse_Iterator operator++(int) {
            auto copy = *this;
            --_iterator;
            return copy;
        }

        Reverse_Iterator operator--(int) {
            auto copy = *this;
            ++_iterator;
            return copy;
        }

        Reverse_Iterator& operator+=(difference_type n) {
            _iterator -= n;
            return *this;
        }

        Reverse_Iterator& operator-=(difference_type n) {
            _iterator += n;
            return *this;
        }

        [[nodiscard]] Reverse_Iterator operator+(difference_type n) {
            return Reverse_Iterator(_iterator - n);
        }

        [[nodiscard]] Reverse_Iterator operator-(difference_type n) {
            return Reverse_Iterator(_iterator + n);
        }

        [[nodiscard]] reference operator*() const {
            return *(_iterator - 1);
        }

        [[nodiscard]] wrapped_iterator_type operator->() const {
            return _iterator - 1;
        }

        [[nodiscard]] reference operator[](difference_type n) const {
            return *(*this + n);
        }

    private:
        wrapped_iterator_type _iterator;
    };

    template<typename T>
    Reverse_Iterator(T) -> Reverse_Iterator<T>;

    template<typename T>
    [[nodiscard]] inline constexpr Reverse_Iterator<T> operator+(typename Reverse_Iterator<T>::difference_type n, Reverse_Iterator<T> const& a) {
        return Reverse_Iterator<T>(a.base() - n);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr auto operator-(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) -> decltype(b.base() - a.base()) {
        return b.base() - a.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator==(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() == b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator!=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() != b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() > b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() < b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() >= b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() <= b.base();
    }

    template<typename Iterator>
    struct Move_Iterator {
    public:
        using value_type = typename Iterator_Traits<Iterator>::value_type;
        using reference = decltype(ANTON_MOV(*anton::declval<Iterator>()));
        using difference_type = typename Iterator_Traits<Iterator>::difference_type;
        using iterator_category = typename Iterator_Traits<Iterator>::iterator_category;
        using wrapped_iterator_type = Iterator;

        explicit Move_Iterator(wrapped_iterator_type const& iterator): _iterator(iterator) {}
        explicit Move_Iterator(wrapped_iterator_type&& iterator): _iterator(ANTON_MOV(iterator)) {}

        wrapped_iterator_type const& base() const {
            return _iterator;
        }

        Move_Iterator& operator++() {
            ++_iterator;
            return *this;
        }

        Move_Iterator& operator--() {
            --_iterator;
            return *this;
        }

        Move_Iterator operator++(int) {
            auto copy = *this;
            ++_iterator;
            return copy;
        }

        Move_Iterator operator--(int) {
            auto copy = *this;
            --_iterator;
            return copy;
        }

        Move_Iterator& operator+=(difference_type n) {
            _iterator += n;
            return *this;
        }

        Move_Iterator& operator-=(difference_type n) {
            _iterator -= n;
            return *this;
        }

        [[nodiscard]] Move_Iterator operator+(difference_type n) {
            return Move_Iterator(_iterator + n);
        }

        [[nodiscard]] Move_Iterator operator-(difference_type n) {
            return Move_Iterator(_iterator - n);
        }

        [[nodiscard]] reference operator*() const {
            return ANTON_MOV(*_iterator);
        }

        [[nodiscard]] reference operator[](difference_type n) const {
            return *(*this + n);
        }

    private:
        wrapped_iterator_type _iterator;
    };

    template<typename T>
    Move_Iterator(T) -> Move_Iterator<T>;

    template<typename T>
    [[nodiscard]] constexpr Move_Iterator<T> operator+(typename Move_Iterator<T>::difference_type n, Move_Iterator<T> const& a) {
        return Move_Iterator<T>(a.base() + n);
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr auto operator-(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) -> decltype(a.base() - b.base()) {
        return a.base() - b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator==(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() == b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator!=(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() != b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator<(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() < b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator>(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() > b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator<=(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() <= b.base();
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator>=(Move_Iterator<T1> const& a, Move_Iterator<T2> const& b) {
        return a.base() >= b.base();
    }
} // namespace anton
