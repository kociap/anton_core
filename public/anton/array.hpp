#pragma once

#include <anton/allocator.hpp>
#include <anton/assert.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>
#include <anton/utility.hpp>

namespace anton {
    // Move constructor of T must not throw any exceptions.
    //
    template<typename T, typename Allocator = Allocator>
    class Array {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = i64;
        using difference_type = isize;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;

        Array();
        explicit Array(Allocator);
        explicit Array(size_type size);
        Array(Reserve_Tag, size_type size);
        Array(size_type, value_type const&);
        Array(Array const& original);
        Array(Array&& from) noexcept;
        template<typename Input_Iterator>
        Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename... Args>
        Array(Variadic_Construct_Tag, Args&&...);
        ~Array();
        Array& operator=(Array const& original);
        Array& operator=(Array&& from) noexcept;

        [[nodiscard]] reference operator[](size_type);
        [[nodiscard]] const_reference operator[](size_type) const;
        [[nodiscard]] pointer data();
        [[nodiscard]] const_pointer data() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] const_iterator cend() const;

        [[nodiscard]] size_type size() const;
        [[nodiscard]] size_type capacity() const;

        // resize
        // Resizes the vector allocating additional memory if n is greater than capacity.
        // If n is greater than size, the new elements are default constructed.
        // If n is less than size, the excess elements are destroyed.
        //
        void resize(size_type n);

        // resize
        // Resizes the vector allocating additional memory if n is greater than capacity.
        // If n is greater than size, the new elements are copy constructed from v.
        // If n is less than size, the excess elements are destroyed.
        //
        void resize(size_type n, value_type const& v);

        // reserve
        // Allocates enough memory to fit n elements of type T.
        // Does nothing if requested_capacity is less than capacity().
        //
        void reserve(size_type n);

        // set_capacity
        // Sets the capacity to exactly match n.
        // If n is not equal capacity, contents are reallocated.
        //
        void set_capacity(size_type n);

        // force_size
        // Changes the size of the vector to n. Useful in situations when the user
        // writes to the vector via external means.
        //
        void force_size(size_type n);

        template<typename Input_Iterator>
        void assign(Input_Iterator first, Input_Iterator last);

        template<typename... Args>
        void insert(Variadic_Construct_Tag, const_iterator position, Args&&... args);
        template<typename... Args>
        void insert(Variadic_Construct_Tag, size_type position, Args&&... args);
        template<typename Input_Iterator>
        void insert(size_type position, Input_Iterator first, Input_Iterator last);
        void insert_unsorted(const_iterator position, value_type const& value);

        void push_back(value_type const&);
        void push_back(value_type&&);
        template<typename... Args>
        reference emplace_back(Args&&... args);

        iterator erase(const_iterator first, const_iterator last);
        void erase_unsorted(size_type index); // TODO: remove
        void erase_unsorted_unchecked(size_type index);
        iterator erase_unsorted(const_iterator first);
        // iterator erase_unsorted(const_iterator first, const_iterator last);

        void pop_back();
        void clear();

    private:
        Allocator _allocator;
        size_type _capacity = 64;
        size_type _size = 0;
        T* _data = nullptr;

        void attempt_copy(T* from, T* to);
        void attempt_move(T* from, T* to);

        T* get_ptr(size_type index = 0);
        T const* get_ptr(size_type index = 0) const;

        T* allocate(size_type);
        void deallocate(void*, size_type);
        void ensure_capacity(size_type requested_capacity);
    };

} // namespace anton

namespace anton {
    template<typename T, typename Allocator>
    Array<T, Allocator>::Array() {
        _data = allocate(_capacity);
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(Allocator alloc): _allocator(move(alloc)) {
        _data = allocate(_capacity);
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(size_type const n) {
        _capacity = math::max(_capacity, n);
        _data = allocate(_capacity);
        uninitialized_default_construct_n(_data, n);
        _size = n;
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(Reserve_Tag, size_type const n): _capacity(n) {
        _data = allocate(_capacity);
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(size_type n, value_type const& value) {
        _capacity = math::max(_capacity, n);
        _data = allocate(_capacity);
        uninitialized_fill_n(_data, n, value);
        _size = n;
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(Array const& v): _allocator(v._allocator), _capacity(v._capacity) {
        _data = allocate(_capacity);
        uninitialized_copy_n(v._data, v._size, _data);
        _size = v._size;
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::Array(Array&& v) noexcept: _allocator(move(v._allocator)), _capacity(v._capacity), _size(v._size), _data(v._data) {
        v._data = nullptr;
        v._capacity = 0;
        v._size = 0;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    Array<T, Allocator>::Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last) {
        size_type const count = last - first;
        _capacity = math::max(_capacity, count);
        _size = count;
        _data = allocate(_capacity);
        uninitialized_copy(first, last, _data);
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    Array<T, Allocator>::Array(Variadic_Construct_Tag, Args&&... args) {
        _capacity = math::max(_capacity, static_cast<size_type>(sizeof...(Args)));
        _data = allocate(_capacity);
        uninitialized_variadic_construct(_data, anton::forward<Args>(args)...);
        _size = static_cast<size_type>(sizeof...(Args));
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>::~Array() {
        if(_data != nullptr) {
            destruct_n(_data, _size);
            deallocate(_data, _capacity);
        }
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>& Array<T, Allocator>::operator=(Array const& v) {
        // TODO: Get rid of this and move to polymorphic
        // static_assert(std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value, "Allocator is not copy assignable");
        _allocator = v._allocator;
        T* new_storage = allocate(v._capacity);
        uninitialized_copy_n(v._data, v._size, new_storage);
        destruct_n(_data, _size);
        // Note: assumes v._allocator == _allocator is true
        deallocate(_data, _capacity);
        _data = new_storage;
        _size = v._size;
        _capacity = v._capacity;
        return *this;
    }

    template<typename T, typename Allocator>
    Array<T, Allocator>& Array<T, Allocator>::operator=(Array&& v) noexcept {
        // Note: We ignore the fact that the allocator_traits<Allocator>::propagate_on_container_swap might be false
        // or the allocators do not compare equal.
        swap(_data, v._data);
        swap(_allocator, v._allocator);
        swap(_capacity, v._capacity);
        swap(_size, v._size);
        return *this;
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::operator[](size_type index) -> reference {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"Index out of bounds.");
        }

        return *get_ptr(index);
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::operator[](size_type index) const -> const_reference {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"Index out of bounds.");
        }

        return *get_ptr(index);
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::data() -> pointer {
        return get_ptr();
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::data() const -> const_pointer {
        return get_ptr();
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::begin() -> iterator {
        return iterator(get_ptr());
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::end() -> iterator {
        return iterator(get_ptr(_size));
    }
    template<typename T, typename Allocator>
    auto Array<T, Allocator>::begin() const -> const_iterator {
        return const_iterator(get_ptr());
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::end() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::cbegin() const -> const_iterator {
        return const_iterator(get_ptr());
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::cend() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::size() const -> size_type {
        return _size;
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::capacity() const -> size_type {
        return _capacity;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::resize(size_type n, value_type const& value) {
        ensure_capacity(n);
        if(n > _size) {
            uninitialized_fill(get_ptr(_size), get_ptr(n), value);
        } else {
            destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::resize(size_type n) {
        ensure_capacity(n);
        if(n > _size) {
            uninitialized_default_construct(get_ptr(_size), get_ptr(n));
        } else {
            destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::reserve(size_type n) {
        ensure_capacity(n);
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::set_capacity(size_type new_capacity) {
        if(new_capacity != _capacity) {
            T* new_data = allocate(new_capacity);
            // TODO: More reallocation options
            if constexpr(is_move_constructible<T>) {
                uninitialized_move_n(_data, math::min(new_capacity, _size), new_data);
            } else {
                uninitialized_copy_n(_data, math::min(new_capacity, _size), new_data);
            }
            destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
            _size = math::min(new_capacity, _size);
        }
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::force_size(size_type n) {
        ANTON_ASSERT(n <= _capacity, u8"Requested size is greater than capacity.");
        _size = n;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    void Array<T, Allocator>::assign(Input_Iterator first, Input_Iterator last) {
        anton::destruct_n(_data, _size);
        _size = 0;
        ensure_capacity(last - first);
        anton::uninitialized_copy(first, last, _data);
        _size = last - first;
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    void Array<T, Allocator>::insert(Variadic_Construct_Tag, const_iterator position, Args&&... args) {
        insert(position - begin(), anton::forward<Args>(args)...);
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    void Array<T, Allocator>::insert(Variadic_Construct_Tag, size_type const position, Args&&... args) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position <= _size && position >= 0, u8"Index out of bounds.");
        }

        if(_size == _capacity || position != _size) {
            if(_size != _capacity) {
                anton::uninitialized_move_n(get_ptr(_size - 1), 1, get_ptr(_size));
                anton::move_backward(get_ptr(position), get_ptr(_size - 1), get_ptr(_size));
                anton::construct(get_ptr(position), anton::forward<Args>(args)...);
                _size += 1;
            } else {
                i64 const new_capacity = _capacity * 2;
                T* const new_data = allocate(new_capacity);
                i64 moved = 0;
                anton::uninitialized_move(get_ptr(0), get_ptr(position), new_data);
                moved = position;
                anton::construct(new_data + position, anton::forward<Args>(args)...);
                moved += 1;
                anton::uninitialized_move(get_ptr(position), get_ptr(_size), new_data + moved);

                anton::destruct_n(_data, _size);
                anton::deallocate(_data, _capacity);
                _capacity = new_capacity;
                _data = new_data;
                _size += 1;
            }
        } else {
            // Quick path when position points to end and we have room for one more element.
            anton::construct(get_ptr(_size), anton::forward<Args>(args)...);
            _size += 1;
        }
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    void Array<T, Allocator>::insert(size_type position, Input_Iterator first, Input_Iterator last) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position <= _size && position >= 0, u8"Index out of bounds.");
        }

        // TODO: Distance and actual support for input iterators.
        i64 const dist = last - first;
        if(_size + dist > _capacity || position != _size) {
            if(_size + dist <= _capacity) {
                i64 const total_elems = _size - position;
                i64 const elems_outside = math::min(_size - position, dist);
                i64 const elems_inside = total_elems - elems_outside;
                i64 const target_offset = math::max(position + dist, _size);
                anton::uninitialized_move_n(get_ptr(position + elems_inside), elems_outside, get_ptr(target_offset));
                anton::move_backward(get_ptr(position), get_ptr(position + elems_inside), get_ptr(position + dist + elems_inside));
                anton::destruct_n(get_ptr(position), elems_inside);
                anton::uninitialized_copy(first, last, get_ptr(position));
                _size += dist;
            } else {
                i64 new_capacity = _capacity * 2;
                while(new_capacity <= _size + dist) {
                    new_capacity *= 2;
                }

                T* const new_data = allocate(new_capacity);
                i64 moved = 0;
                anton::uninitialized_move(get_ptr(0), get_ptr(position), new_data);
                moved = position;
                anton::uninitialized_copy(first, last, new_data + moved);
                moved += dist;
                anton::uninitialized_move(get_ptr(position), get_ptr(_size), new_data + moved);

                destruct_n(_data, _size);
                deallocate(_data, _capacity);
                _capacity = new_capacity;
                _data = new_data;
                _size += dist;
            }
        } else {
            // Quick path when position points to end and we have room for dist elements.
            anton::uninitialized_copy(first, last, get_ptr(_size));
            _size += dist;
        }
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::insert_unsorted(const_iterator position, value_type const& value) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position <= _size && position >= 0, u8"Index out of bounds.");
        }

        ensure_capacity(_size + 1);
        size_type offset = static_cast<size_type>(position - _data);
        if(offset == _size) {
            construct(get_ptr(offset), value);
            ++_size;
        } else {
            T* elem_ptr = get_ptr(offset);
            if constexpr(is_move_constructible<T>) {
                attempt_move(elem_ptr, get_ptr(_size));
            } else {
                attempt_copy(elem_ptr, get_ptr(_size));
            }
            destruct(elem_ptr);
            construct(elem_ptr, value);
            ++_size;
        }
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::push_back(value_type const& val) {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        construct(elem_ptr, val);
        ++_size;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::push_back(value_type&& val) {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        construct(elem_ptr, move(val));
        ++_size;
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    auto Array<T, Allocator>::emplace_back(Args&&... args) -> reference {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        construct(elem_ptr, anton::forward<Args>(args)...);
        ++_size;
        return *elem_ptr;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::erase_unsorted(size_type index) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index <= size && index >= 0, u8"Index out of range.");
        }

        erase_unsorted_unchecked(index);
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::erase_unsorted_unchecked(size_type index) {
        T* elem_ptr = get_ptr(index);
        T* last_elem_ptr = get_ptr(_size - 1);
        if(elem_ptr != last_elem_ptr) { // Prevent self move-assignment
            *elem_ptr = move(*last_elem_ptr);
        }
        destruct(last_elem_ptr);
        --_size;
    }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::erase_unsorted(const_iterator iter) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            // TODO: Implement.
        }

        auto pos = const_cast<value_type*>(iter);
        if(iter != cend() - 1) {
            *pos = move(*get_ptr(_size - 1));
        }
        destruct(get_ptr(_size - 1));
        _size -= 1;
        return pos;
    }

    //     template <typename T, typename Allocator>
    //     auto Array<T, Allocator>::erase_unsorted(const_iterator first, const_iterator last) -> iterator {
    // #if ANTON_ITERATOR_DEBUG

    // #endif // ANTON_ITERATOR_DEBUG

    //         if (first != last) {
    //             auto first_last_elems = last - first;
    //             auto last_end_elems = end() - last;
    //             auto elems_till_end = math::min(first_last_elems, last_end_elems);
    //             move(end() - elems_till_end, end(), first);
    //             destruct(end() - elems_till_end, end());
    //             _size -= first_last_elems;
    //         }

    //         return first;
    //     }

    template<typename T, typename Allocator>
    auto Array<T, Allocator>::erase(const_iterator first, const_iterator last) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            // TODO: Implement.
        }

        if(first != last) {
            iterator pos = move(const_cast<value_type*>(last), end(), const_cast<value_type*>(first));
            destruct(pos, end());
            _size -= last - first;
        }

        return const_cast<value_type*>(first);
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::pop_back() {
        ANTON_VERIFY(_size > 0, u8"Trying to pop an element from an empty Array.");
        T* last_elem_ptr = get_ptr(_size - 1);
        destruct(last_elem_ptr);
        --_size;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::clear() {
        destruct(get_ptr(), get_ptr(_size));
        _size = 0;
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::attempt_copy(T* from, T* to) {
        ::new(to) T(*from);
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::attempt_move(T* from, T* to) {
        ::new(to) T(move(*from));
    }

    template<typename T, typename Allocator>
    T* Array<T, Allocator>::get_ptr(size_type index) {
        return launder(_data + index);
    }

    template<typename T, typename Allocator>
    T const* Array<T, Allocator>::get_ptr(size_type index) const {
        return launder(_data + index);
    }

    template<typename T, typename Allocator>
    T* Array<T, Allocator>::allocate(size_type const size) {
        void* mem = _allocator.allocate(size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
        return static_cast<T*>(mem);
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::deallocate(void* mem, size_type const size) {
        _allocator.deallocate(mem, size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
    }

    template<typename T, typename Allocator>
    void Array<T, Allocator>::ensure_capacity(size_type requested_capacity) {
        if(requested_capacity > _capacity) {
            size_type new_capacity = _capacity;
            while(new_capacity < requested_capacity) {
                new_capacity *= 2;
            }

            T* new_data = allocate(new_capacity);
            // TODO: More reallocation options
            if constexpr(is_move_constructible<T>) {
                uninitialized_move(_data, _data + _size, new_data);
            } else {
                uninitialized_copy(_data, _data + _size, new_data);
            }
            destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
        }
    }
} // namespace anton
