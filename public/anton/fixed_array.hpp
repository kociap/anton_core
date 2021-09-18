#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/assert.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>
#include <anton/types.hpp>

namespace anton {
    template<typename T, i64 Capacity>
    struct Fixed_Array {
        static_assert(Capacity > 0, u8"Fixed_Array's capacity must be greater than 0");

    public:
        using value_type = T;
        using size_type = i64;
        using difference_type = i64;
        using iterator = T*;
        using const_iterator = T const*;

        Fixed_Array();
        explicit Fixed_Array(size_type size);
        Fixed_Array(Reserve_Tag, size_type size);
        Fixed_Array(size_type, value_type const&);
        Fixed_Array(Fixed_Array const& original);
        Fixed_Array(Fixed_Array&& from);
        template<typename Input_Iterator>
        Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename... Args>
        Fixed_Array(Variadic_Construct_Tag, Args&&...);
        ~Fixed_Array();
        Fixed_Array& operator=(Fixed_Array const& original);
        Fixed_Array& operator=(Fixed_Array&& from);

        [[nodiscard]] T& operator[](size_type index);
        [[nodiscard]] T const& operator[](size_type index) const;

        // back
        // Accesses the last element of the array. The behaviour is undefined when the array is empty.
        //
        [[nodiscard]] T& back();
        [[nodiscard]] T const& back() const;

        [[nodiscard]] T* data();
        [[nodiscard]] T const* data() const;

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

        T& push_back(value_type const&);
        T& push_back(value_type&&);
        template<typename... Args>
        T& emplace_back(Args&&... args);

        void pop_back();

        friend void swap(Fixed_Array& a1, Fixed_Array& a2) {
            Fixed_Array tmp = ANTON_MOV(a1);
            a1 = ANTON_MOV(a2);
            a2 = ANTON_MOV(tmp);
        }

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> _data[Capacity];
        size_type _size;
    };
} // namespace anton

namespace anton {
    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(): _size(0) {}

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s): _size(s) {
        ANTON_VERIFY(s <= Capacity, u8"size is greater than capacity");
        anton::uninitialized_default_construct_n(reinterpret_cast<T*>(_data), _size);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s, value_type const& v): _size(s) {
        ANTON_VERIFY(s <= Capacity, u8"size is greater than capacity");
        anton::uninitialized_fill_n(reinterpret_cast<T*>(_data), _size, v);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array const& other): _size(other._size) {
        anton::uninitialized_copy_n(reinterpret_cast<T const*>(other._data), _size, reinterpret_cast<T*>(_data));
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array&& other): _size(other._size) {
        anton::uninitialized_move_n(reinterpret_cast<T*>(other._data), _size, reinterpret_cast<T*>(_data));
    }

    template<typename T, i64 Capacity>
    template<typename Input_Iterator>
    Fixed_Array<T, Capacity>::Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last) {
        i64 const distance = last - first;
        ANTON_VERIFY(distance <= Capacity, u8"distance between last and first is greater than the capacity");
        anton::uninitialized_copy(first, last, reinterpret_cast<T*>(_data));
    }

    template<typename T, i64 Capacity>
    template<typename... Args>
    Fixed_Array<T, Capacity>::Fixed_Array(Variadic_Construct_Tag, Args&&... args): _size(sizeof...(Args)) {
        static_assert(sizeof...(Args) <= Capacity, u8"attempting to construct Fixed_Array with more elements than capacity");
        anton::uninitialized_variadic_construct(reinterpret_cast<T*>(_data), ANTON_FWD(args)...);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::~Fixed_Array() {
        anton::destruct_n(reinterpret_cast<T*>(_data), _size);
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array const& other) {
#define PTR(data, offset) reinterpret_cast<T*>(data + offset)
#define PTRC(data, offset) reinterpret_cast<T const*>(data + offset)
        if(_size >= other._size) {
            anton::copy(PTRC(other._data, 0), PTRC(other._data, other._size), PTR(_data, 0));
            anton::destruct_n(PTR(_data, other._size), _size - other._size);
            _size = other._size;
        } else {
            anton::copy(PTRC(other._data, 0), PTRC(other._data, _size), PTR(_data, 0));
            anton::uninitialized_copy(PTRC(other._data, _size), PTRC(other._data, other._size), PTR(_data, _size));
            _size = other._size;
        }
        return *this;
#undef PTR
    }

    template<typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array&& other) {
#define PTR(data, offset) reinterpret_cast<T*>(data + offset)
        if(_size >= other._size) {
            anton::move(PTR(other._data, 0), PTR(other._data, other._size), PTR(_data, 0));
            anton::destruct_n(PTR(_data, other._size), _size - other._size);
            _size = other._size;
        } else {
            anton::move(PTR(other._data, 0), PTR(other._data, _size), PTR(_data, 0));
            anton::uninitialized_move(PTR(other._data, _size), PTR(other._data, other._size), PTR(_data, _size));
            _size = other._size;
        }
        return *this;
#undef PTR
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) -> T& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"index out of bounds");
        }

        return *reinterpret_cast<T*>(_data + index);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) const -> T const& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"index out of bounds");
        }

        return *reinterpret_cast<T const*>(_data + index);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::back() -> T& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(_size > 0, u8"attempting to call back() on empty Array");
        }

        return *reinterpret_cast<T*>(_data + _size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::back() const -> T const& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(_size > 0, u8"attempting to call back() on empty Array");
        }

        return *reinterpret_cast<T const*>(_data + _size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() -> T* {
        return reinterpret_cast<T*>(_data);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() const -> T const* {
        return reinterpret_cast<T const*>(_data);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() -> iterator {
        return reinterpret_cast<T*>(_data);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() -> iterator {
        return reinterpret_cast<T*>(_data + _size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() const -> const_iterator {
        return reinterpret_cast<T const*>(_data);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() const -> const_iterator {
        return reinterpret_cast<T const*>(_data + _size);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cbegin() const -> const_iterator {
        return reinterpret_cast<T const*>(_data);
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cend() const -> const_iterator {
        return reinterpret_cast<T const*>(_data + _size);
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
        ANTON_VERIFY(s <= Capacity && s >= 0, u8"requested size was outside the range [0, capacity()]");
        if(s >= _size) {
            anton::uninitialized_default_construct_n(reinterpret_cast<T*>(_data + _size), s - _size);
        } else {
            anton::destruct_n(reinterpret_cast<T*>(_data + s), _size - s);
        }
        _size = s;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::resize(size_type const s, T const& v) {
        ANTON_VERIFY(s <= Capacity && s >= 0, u8"requested size was outside the range [0, capacity()]");
        if(s >= _size) {
            anton::uninitialized_fill_n(reinterpret_cast<T*>(_data + _size), s - _size, v);
        } else {
            anton::destruct_n(reinterpret_cast<T*>(_data + s), _size - s);
        }
        _size = s;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::clear() {
        anton::destruct_n(reinterpret_cast<T*>(_data), _size);
        _size = 0;
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::push_back(value_type const& value) -> T& {
        ANTON_VERIFY(_size < Capacity, u8"cannot emplace_back element in a full Fixed_Array");
        T* const element = reinterpret_cast<T*>(_data + _size);
        anton::construct(element, value);
        _size += 1;
        return *element;
    }

    template<typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::push_back(value_type&& value) -> T& {
        ANTON_VERIFY(_size < Capacity, u8"cannot emplace_back element in a full Fixed_Array");
        T* const element = reinterpret_cast<T*>(_data + _size);
        anton::construct(element, ANTON_MOV(value));
        _size += 1;
        return *element;
    }

    template<typename T, i64 Capacity>
    template<typename... Args>
    auto Fixed_Array<T, Capacity>::emplace_back(Args&&... args) -> T& {
        ANTON_VERIFY(_size < Capacity, u8"cannot emplace_back element in a full Fixed_Array");
        T* const element = reinterpret_cast<T*>(_data + _size);
        anton::construct(element, ANTON_FWD(args)...);
        _size += 1;
        return *element;
    }

    template<typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::pop_back() {
        ANTON_VERIFY(_size > 0, u8"trying to pop an element from an empty Fixed_Array");
        T* const elem = reinterpret_cast<T*>(_data + _size - 1);
        anton::destruct(elem);
        _size -= 1;
    }
} // namespace anton
