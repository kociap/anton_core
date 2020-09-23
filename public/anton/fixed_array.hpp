#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/assert.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>
#include <anton/types.hpp>

namespace anton {
    template<typename T, i64 Capacity>
    class Fixed_Array {
        static_assert(Capacity >= 0, u8"Fixed_Array's capacity may not be less than 0");

    public:
        using value_type = T;
        using size_type = i64;
        using difference_type = isize;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;

        Fixed_Array();
        explicit Fixed_Array(size_type size);
        Fixed_Array(Reserve_Tag, size_type size);
        Fixed_Array(size_type, value_type const&);
        Fixed_Array(Fixed_Array const& original);
        Fixed_Array(Fixed_Array&& from) noexcept;
        template<typename Input_Iterator>
        Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename... Args>
        Fixed_Array(Variadic_Construct_Tag, Args&&...);
        ~Fixed_Array();
        Fixed_Array& operator=(Fixed_Array const& original);
        Fixed_Array& operator=(Fixed_Array&& from) noexcept;

        [[nodiscard]] reference operator[](size_type index);
        [[nodiscard]] const_reference operator[](size_type index) const;
        [[nodiscard]] pointer data();
        [[nodiscard]] const_pointer data() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] const_iterator cend() const;

        // size
        // The number of elements contained in the array.
        //
        [[nodiscard]] size_type size() const;

        // size_bytes
        // The size of all the elements contained in the array in bytes.
        // Equivalent to 'sizeof(T) * size()'.
        //
        [[nodiscard]] size_type size_bytes() const;

        [[nodiscard]] size_type capacity() const;

        void resize(size_type count);
        void resize(size_type count, T const& value);
        void clear();
        template<typename... Args>
        reference emplace_back(Args&&... args);
        void pop_back();

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> _data[Capacity];
        size_type _size;

        T* get_ptr(size_type);
        T const* get_ptr(size_type) const;

        friend void swap(Fixed_Array& a1, Fixed_Array& a2) {
            Fixed_Array tmp = ANTON_MOV(a1);
            a1 = ANTON_MOV(a2);
            a2 = ANTON_MOV(tmp);
        }
    };
} // namespace anton

namespace anton {
    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(): _size(0) {}

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s): _size(s) {
        ANTON_VERIFY(s <= Capacity, u8"Size is greater than capacity.");
        uninitialized_default_construct_n(get_ptr(0), _size);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s, value_type const& v): _size(s) {
        ANTON_VERIFY(s <= Capacity, u8"Size is greater than capacity.");
        uninitialized_fill_n(get_ptr(0), _size, v);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array const& other): _size(other._size) {
        uninitialized_copy_n(other.get_ptr(0), _size, get_ptr(0));
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array&& other) noexcept: _size(other._size) {
        uninitialized_move_n(other.get_ptr(0), _size, get_ptr(0));
    }

    template<typename T, i64 Capacity>
    template<typename Input_Iterator>
    Fixed_Array<T, Capacity>::Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last) {
        i64 const distance = last - first;
        ANTON_VERIFY(distance <= Capacity, u8"Distance between last and first is greater than the capacity.");
        uninitialized_copy(first, last, get_ptr(0));
    }

    template<typename T, i64 Capacity>
    template<typename... Args>
    Fixed_Array<T, Capacity>::Fixed_Array(Variadic_Construct_Tag, Args&&... args): _size(sizeof...(Args)) {
        static_assert(sizeof...(Args) <= Capacity, u8"Attempting to construct Fixed_Array with more elements than capacity.");
        uninitialized_variadic_construct(get_ptr(0), ANTON_FWD<Args>(args)...);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::~Fixed_Array() {
        destruct_n(get_ptr(0), _size);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array const& other) {
        if(_size >= other._size) {
            copy(other.get_ptr(0), other.get_ptr(other._size), get_ptr(0));
            destruct_n(get_ptr(other._size), _size - other._size);
            _size = other._size;
        } else {
            copy(other.get_ptr(0), other.get_ptr(_size), get_ptr(0));
            uninitialized_copy(other.get_ptr(_size), other.get_ptr(other._size), get_ptr(_size));
            _size = other._size;
        }
        return *this;
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array&& from) noexcept {
        if(_size >= from._size) {
            move(from.get_ptr(0), from.get_ptr(from._size), get_ptr(0));
            destruct_n(get_ptr(from._size), _size - from._size);
            _size = from._size;
        } else {
            move(from.get_ptr(0), from.get_ptr(_size), get_ptr(0));
            uninitialized_move(from.get_ptr(_size), from.get_ptr(from._size), get_ptr(_size));
            _size = from._size;
        }
        return *this;
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) -> reference {
        return *get_ptr(index);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) const -> const_reference {
        return *get_ptr(index);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() -> pointer {
        return get_ptr(0);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() const -> const_pointer {
        return get_ptr(0);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() -> iterator {
        return get_ptr(0);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() -> iterator {
        return get_ptr(_size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() const -> const_iterator {
        return get_ptr(0);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() const -> const_iterator {
        return get_ptr(_size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cbegin() const -> const_iterator {
        return get_ptr(0);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cend() const -> const_iterator {
        return get_ptr(_size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::size() const -> size_type {
        return _size;
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::size_bytes() const -> size_type {
        return _size * sizeof(T);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::capacity() const -> size_type {
        return Capacity;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::resize(size_type const s) {
        ANTON_VERIFY(s <= Capacity && s >= 0, u8"Requested size was outside the range [0, capacity()].");
        if(s >= _size) {
            uninitialized_default_construct_n(get_ptr(_size), s - _size);
        } else {
            destruct_n(get_ptr(s), _size - s);
        }
        _size = s;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::resize(size_type const s, T const& v) {
        ANTON_VERIFY(s <= Capacity && s >= 0, u8"Requested size was outside the range [0, capacity()].");
        if(s >= _size) {
            uninitialized_fill_n(get_ptr(_size), s - _size);
        } else {
            destruct_n(get_ptr(s), _size - s);
        }
        _size = s;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::clear() {
        destruct_n(get_ptr(0), _size);
        _size = 0;
    }

    template<typename T, i64 Capacity>
    template<typename... Args>
    auto Fixed_Array<T, Capacity>::emplace_back(Args&&... args) -> reference {
        ANTON_VERIFY(_size < Capacity, u8"Cannot emplace_back element in a full Fixed_Array.");
        T* const elem = get_ptr(_size);
        construct(elem, ANTON_FWD<Args>(args)...);
        _size += 1;
        return *elem;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::pop_back() {
        ANTON_VERIFY(_size > 0, u8"Trying to pop an element from an empty Fixed_Array");
        T* const elem = get_ptr(_size - 1);
        destruct(elem);
        _size -= 1;
    }

    template<typename T, i64 Capacity>
    T* Fixed_Array<T, Capacity>::get_ptr(size_type const index) {
        return launder(reinterpret_cast<T*>(_data + index));
    }

    template<typename T, i64 Capacity>
    T const* Fixed_Array<T, Capacity>::get_ptr(size_type const index) const {
        return launder(reinterpret_cast<T const*>(_data + index));
    }
} // namespace anton
