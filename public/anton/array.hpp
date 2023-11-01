#pragma once

#include <anton/allocator.hpp>
#include <anton/assert.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>
#include <anton/utility.hpp>

namespace anton {
#define ANTON_ARRAY_MIN_ALLOCATION_SIZE ((i64)64)

    template<typename T>
    struct Array {
    public:
        using value_type = T;
        using allocator_type = Polymorphic_Allocator;
        using size_type = i64;
        using difference_type = i64;
        using iterator = T*;
        using const_iterator = T const*;

        Array();
        explicit Array(allocator_type const& allocator);
        // Construct an array with n default constructed elements
        explicit Array(size_type n);
        // Construct an array with n default constructed elements
        explicit Array(allocator_type const& allocator, size_type n);
        // Construct an array with n copies of value
        explicit Array(size_type n, value_type const& value);
        // Construct an array with n copies of value
        explicit Array(allocator_type const& allocator, size_type n, value_type const& value);
        // Construct an array with capacity to fit at least n elements
        explicit Array(Reserve_Tag, size_type n);
        // Construct an array with capacity to fit at least n elements
        explicit Array(allocator_type const& allocator, Reserve_Tag, size_type n);
        // Copies the allocator
        Array(Array const& other);
        Array(allocator_type const& allocator, Array const& other);
        // Moves the allocator
        Array(Array&& other);
        // Array
        // If the allocators do not compare equal, allocates new memory and
        // moves the objects.
        //
        // Complexity:
        // O(n) if allocators compare unequal.
        // O(1) otherwise.
        //
        Array(allocator_type const& allocator, Array&& other);
        template<typename Input_Iterator>
        Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename Input_Iterator>
        Array(allocator_type const& allocator, Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename... Args>
        Array(Variadic_Construct_Tag, Args&&...);
        template<typename... Args>
        Array(allocator_type const& allocator, Variadic_Construct_Tag, Args&&...);
        ~Array();

        Array& operator=(Array const& other);
        Array& operator=(Array&& other);

        [[nodiscard]] T& operator[](size_type);
        [[nodiscard]] T const& operator[](size_type) const;

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

        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        // resize
        // Resizes the array allocating additional memory if n is greater than capacity.
        // If n is greater than size, the new elements are default constructed.
        // If n is less than size, the excess elements are destroyed.
        //
        void resize(size_type n);

        // resize
        // Resizes the array allocating additional memory if n is greater than capacity.
        // If n is greater than size, the new elements are copy constructed from v.
        // If n is less than size, the excess elements are destroyed.
        //
        void resize(size_type n, value_type const& v);

        // ensure_capacity
        // Allocates enough memory to fit requested_capacity elements of type T.
        // Does nothing if requested_capacity is less than capacity().
        //
        void ensure_capacity(size_type requested_capacity);

        // set_capacity
        // Sets the capacity to exactly match n.
        // If n is not equal capacity, contents are reallocated.
        //
        void set_capacity(size_type n);

        // force_size
        // Changes the size of the array to n. Useful in situations when the user
        // writes to the array via external means.
        //
        void force_size(size_type n);

        // assign
        // Overwrite the contents of the array with elements from the range [first, last[.
        //
        // Parameters:
        // first, last - the range of elements to replace the contents with.
        //
        template<typename Input_Iterator>
        void assign(Input_Iterator first, Input_Iterator last);

        // insert
        // Inserts an object into the array at position.
        //
        // Parameters:
        // position - iterator to the insert position. Must be a valid iterator.
        //    value - the object to be inserted into the array.
        //
        // Returns:
        // iterator to the inserted element.
        //
        iterator insert(const_iterator position, T const& value);
        iterator insert(const_iterator position, T&& value);

        // insert
        // Constructs an object directly into array at position avoiding copies or moves.
        //
        // Parameters:
        // position - iterator to the insert position. Must be a valid iterator.
        //  args... - arguments to forward to the constructor of T.
        //
        // Returns:
        // iterator to the inserted element.
        //
        template<typename... Args>
        iterator insert(Variadic_Construct_Tag, const_iterator position, Args&&... args);

        // insert
        // Constructs an object directly into array at position avoiding copies or moves.
        // position must be an index greater than or equal 0 and less than or equal size.
        //
        // Returns:
        // iterator to the inserted element.
        //
        template<typename... Args>
        iterator insert(Variadic_Construct_Tag, size_type position, Args&&... args);

        // insert
        // Insert a range of elements into array at position.
        // position must be a valid iterator.
        //
        // Returns:
        // iterator to the first of the inserted elements.
        //
        template<typename Input_Iterator>
        iterator insert(const_iterator position, Input_Iterator first, Input_Iterator last);

        // insert
        // Insert a range of elements into array at position.
        // position must be an index greater than or equal 0 and less than or equal size.
        //
        // Returns:
        // iterator to the first of the inserted elements.
        //
        template<typename Input_Iterator>
        iterator insert(size_type position, Input_Iterator first, Input_Iterator last);

        // insert_unsorted
        // Inserts an element into array by moving the object at position to the end of the array
        // and then copying value into position.
        // position must be a valid iterator.
        //
        // Returns:
        // iterator to the inserted element.
        //
        iterator insert_unsorted(const_iterator position, value_type const& value);
        iterator insert_unsorted(const_iterator position, value_type&& value);

        // insert_unsorted
        // Inserts an element into array by moving the object at position to the end of the array
        // and then copying value into position.
        // position must be an index greater than or equal 0 and less than or equal size.
        //
        // Returns:
        // iterator to the inserted element.
        //
        iterator insert_unsorted(size_type position, value_type const& value);
        iterator insert_unsorted(size_type position, value_type&& value);

        T& push_back(value_type const&);
        T& push_back(value_type&&);
        template<typename... Args>
        T& emplace_back(Args&&... args);

        iterator erase(const_iterator first, const_iterator last);
        void erase_unsorted(size_type index);
        void erase_unsorted_unchecked(size_type index);
        iterator erase_unsorted(const_iterator first);
        // iterator erase_unsorted(const_iterator first, const_iterator last);

        void pop_back();
        void clear();

        // swap
        // Exchanges the contents of the two arrays without copying,
        // moving or swapping the individual elements.
        // Exchanges the allocators.
        //
        // Parameters:
        // lhs, rhs - the containers to exchange the contents of.
        //
        // Complexity:
        // Constant.
        //
        friend void swap(Array& lhs, Array& rhs) {
            // We do not follow the C++ Standard in its complex ways to swap containers
            // and always swap both the allocator and the memory since it is a common expectation
            // of the users, is much faster than copying all the elements and does not break
            // the container or put it in an invalid state.
            using anton::swap;
            swap(lhs._allocator, rhs._allocator);
            swap(lhs._capacity, rhs._capacity);
            swap(lhs._size, rhs._size);
            swap(lhs._data, rhs._data);
        }

    private:
        allocator_type _allocator;
        size_type _capacity = 0;
        size_type _size = 0;
        T* _data = nullptr;

        T* allocate(size_type);
        void deallocate(void*, size_type);
    };
} // namespace anton

namespace anton {
    template<typename T>
    Array<T>::Array(): _allocator() {}

    template<typename T>
    Array<T>::Array(allocator_type const& allocator): _allocator(allocator) {}

    template<typename T>
    Array<T>::Array(size_type const n): Array(n, allocator_type()) {}

    template<typename T>
    Array<T>::Array(allocator_type const& allocator, size_type const n): _allocator(allocator) {
        _capacity = math::max(ANTON_ARRAY_MIN_ALLOCATION_SIZE, n);
        _data = allocate(_capacity);
        anton::uninitialized_default_construct_n(_data, n);
        _size = n;
    }

    template<typename T>
    Array<T>::Array(size_type n, value_type const& value): Array(n, value, allocator_type()) {}

    template<typename T>
    Array<T>::Array(allocator_type const& allocator, size_type n, value_type const& value): _allocator(allocator) {
        _capacity = math::max(ANTON_ARRAY_MIN_ALLOCATION_SIZE, n);
        _data = allocate(_capacity);
        anton::uninitialized_fill_n(_data, n, value);
        _size = n;
    }

    template<typename T>
    Array<T>::Array(Reserve_Tag, size_type const n): Array(reserve, n, allocator_type()) {}

    template<typename T>
    Array<T>::Array(allocator_type const& allocator, Reserve_Tag, size_type const n): _allocator(allocator) {
        _capacity = math::max(ANTON_ARRAY_MIN_ALLOCATION_SIZE, n);
        _data = allocate(_capacity);
    }

    template<typename T>
    Array<T>::Array(Array const& other): Array(allocator_type(), other) {
        if(_capacity > 0) {
            _data = allocate(_capacity);
            anton::uninitialized_copy_n(other._data, other._size, _data);
            _size = other._size;
        }
    }

    template<typename T>
    Array<T>::Array(allocator_type const& allocator, Array const& other): _allocator(allocator), _capacity(other._capacity) {
        if(_capacity > 0) {
            _data = allocate(_capacity);
            anton::uninitialized_copy_n(other._data, other._size, _data);
            _size = other._size;
        }
    }

    template<typename T>
    Array<T>::Array(Array&& other): _allocator(ANTON_MOV(other._allocator)), _capacity(other._capacity), _size(other._size), _data(other._data) {
        other._data = nullptr;
        other._capacity = 0;
        other._size = 0;
    }

    template<typename T>
    Array<T>::Array(allocator_type const& allocator, Array&& other): _allocator(allocator), _capacity(other._capacity), _size(other._size) {
        if(allocator == other._allocator) {
            _data = other._data;
        } else {
            _data = allocate(_capacity);
            // TODO: Assumes elements are movable.
            anton::uninitialized_move_n(other._data, other._size, _data);
            anton::destruct_n(other._data, other._size);
            other.deallocate(other._data, other._capacity);
        }

        other._data = nullptr;
        other._capacity = 0;
        other._size = 0;
        other._allocator = allocator_type();
    }

    template<typename T>
    template<typename Input_Iterator>
    Array<T>::Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last)
        : Array(allocator_type(), range_construct, ANTON_MOV(first), ANTON_MOV(last)) {}

    template<typename T>
    template<typename Input_Iterator>
    Array<T>::Array(allocator_type const& allocator, Range_Construct_Tag, Input_Iterator first, Input_Iterator last): _allocator(allocator) {
        // TODO: Use distance?
        size_type const count = last - first;
        _capacity = math::max(ANTON_ARRAY_MIN_ALLOCATION_SIZE, count);
        _data = allocate(_capacity);
        anton::uninitialized_copy(first, last, _data);
        _size = count;
    }

    template<typename T>
    template<typename... Args>
    Array<T>::Array(Variadic_Construct_Tag, Args&&... args): Array(allocator_type(), variadic_construct, ANTON_FWD(args)...) {}

    template<typename T>
    template<typename... Args>
    Array<T>::Array(allocator_type const& allocator, Variadic_Construct_Tag, Args&&... args): _allocator(allocator) {
        _capacity = math::max(ANTON_ARRAY_MIN_ALLOCATION_SIZE, static_cast<size_type>(sizeof...(Args)));
        _data = allocate(_capacity);
        anton::uninitialized_variadic_construct(_data, ANTON_FWD(args)...);
        _size = static_cast<size_type>(sizeof...(Args));
    }

    template<typename T>
    Array<T>::~Array() {
        anton::destruct_n(_data, _size);
        deallocate(_data, _capacity);
    }

    template<typename T>
    Array<T>& Array<T>::operator=(Array const& other) {
        anton::destruct_n(_data, _size);
        // TODO: Do not deallocate if capacity equal. Consider not deallocating
        // at all when current capacity > other capacity.
        deallocate(_data, _capacity);
        _capacity = other._capacity;
        _size = other._size;
        _allocator = other._allocator;
        if(_capacity > 0) {
            _data = allocate(_capacity);
            anton::uninitialized_copy_n(other._data, other._size, _data);
        }
        return *this;
    }

    template<typename T>
    Array<T>& Array<T>::operator=(Array&& other) {
        swap(*this, other);
        return *this;
    }

    template<typename T>
    auto Array<T>::operator[](size_type index) -> T& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"index out of bounds");
        }

        return _data[index];
    }

    template<typename T>
    auto Array<T>::operator[](size_type index) const -> T const& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"index out of bounds");
        }

        return _data[index];
    }

    template<typename T>
    auto Array<T>::back() -> T& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(_size > 0, u8"attempting to call back() on empty Array");
        }

        return _data[_size - 1];
    }

    template<typename T>
    auto Array<T>::back() const -> T const& {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(_size > 0, u8"attempting to call back() on empty Array");
        }

        return _data[_size - 1];
    }

    template<typename T>
    auto Array<T>::data() -> T* {
        return _data;
    }

    template<typename T>
    auto Array<T>::data() const -> T const* {
        return _data;
    }

    template<typename T>
    auto Array<T>::begin() -> iterator {
        return _data;
    }

    template<typename T>
    auto Array<T>::end() -> iterator {
        return _data + _size;
    }
    template<typename T>
    auto Array<T>::begin() const -> const_iterator {
        return _data;
    }

    template<typename T>
    auto Array<T>::end() const -> const_iterator {
        return _data + _size;
    }

    template<typename T>
    auto Array<T>::cbegin() const -> const_iterator {
        return _data;
    }

    template<typename T>
    auto Array<T>::cend() const -> const_iterator {
        return _data + _size;
    }

    template<typename T>
    auto Array<T>::size() const -> size_type {
        return _size;
    }

    template<typename T>
    auto Array<T>::size_bytes() const -> size_type {
        return _size * sizeof(T);
    }

    template<typename T>
    auto Array<T>::capacity() const -> size_type {
        return _capacity;
    }

    template<typename T>
    auto Array<T>::get_allocator() -> allocator_type& {
        return _allocator;
    }

    template<typename T>
    auto Array<T>::get_allocator() const -> allocator_type const& {
        return _allocator;
    }

    template<typename T>
    void Array<T>::resize(size_type n, value_type const& value) {
        ensure_capacity(n);
        if(n > _size) {
            anton::uninitialized_fill(_data + _size, _data + n, value);
        } else {
            anton::destruct(_data + n, _data + _size);
        }
        _size = n;
    }

    template<typename T>
    void Array<T>::resize(size_type n) {
        ensure_capacity(n);
        if(n > _size) {
            anton::uninitialized_default_construct(_data + _size, _data + n);
        } else {
            anton::destruct(_data + n, _data + _size);
        }
        _size = n;
    }

    template<typename T>
    void Array<T>::ensure_capacity(size_type requested_capacity) {
        if(requested_capacity > _capacity) {
            size_type new_capacity = (_capacity > 0 ? _capacity : ANTON_ARRAY_MIN_ALLOCATION_SIZE);
            while(new_capacity < requested_capacity) {
                new_capacity *= 2;
            }

            T* new_data = allocate(new_capacity);
            if constexpr(is_move_constructible<T>) {
                uninitialized_move(_data, _data + _size, new_data);
            } else {
                uninitialized_copy(_data, _data + _size, new_data);
            }
            anton::destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
        }
    }

    template<typename T>
    void Array<T>::set_capacity(size_type new_capacity) {
        ANTON_ASSERT(new_capacity >= 0, "capacity must be greater than or equal 0");
        if(new_capacity != _capacity) {
            i64 const new_size = math::min(new_capacity, _size);
            T* new_data = nullptr;
            if(new_capacity > 0) {
                new_data = allocate(new_capacity);
            }

            if constexpr(is_move_constructible<T>) {
                anton::uninitialized_move_n(_data, new_size, new_data);
            } else {
                anton::uninitialized_copy_n(_data, new_size, new_data);
            }

            anton::destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
            _size = new_size;
        }
    }

    template<typename T>
    void Array<T>::force_size(size_type n) {
        ANTON_ASSERT(n <= _capacity, u8"requested size is greater than capacity");
        _size = n;
    }

    template<typename T>
    template<typename Input_Iterator>
    void Array<T>::assign(Input_Iterator first, Input_Iterator last) {
        anton::destruct_n(_data, _size);
        ensure_capacity(last - first);
        anton::uninitialized_copy(first, last, _data);
        _size = last - first;
    }

    template<typename T>
    auto Array<T>::insert(const_iterator position, T const& value) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert(variadic_construct, offset, value);
    }

    template<typename T>
    auto Array<T>::insert(const_iterator position, T&& value) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert(variadic_construct, offset, ANTON_MOV(value));
    }

    template<typename T>
    template<typename... Args>
    auto Array<T>::insert(Variadic_Construct_Tag, const_iterator position, Args&&... args) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert(variadic_construct, offset, ANTON_FWD(args)...);
    }

    template<typename T>
    template<typename... Args>
    auto Array<T>::insert(Variadic_Construct_Tag, size_type const position, Args&&... args) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position >= 0 && position <= _size, u8"index out of bounds");
        }

        if(_size == _capacity || position != _size) {
            if(_size != _capacity) {
                anton::uninitialized_move_n(_data + _size - 1, 1, _data + _size);
                anton::move_backward(_data + position, _data + _size - 1, _data + _size);
                anton::construct(_data + position, ANTON_FWD(args)...);
                _size += 1;
            } else {
                i64 const new_capacity = (_capacity > 0 ? _capacity * 2 : ANTON_ARRAY_MIN_ALLOCATION_SIZE);
                T* const new_data = allocate(new_capacity);
                i64 moved = 0;
                anton::uninitialized_move(_data, _data + position, new_data);
                moved = position;
                anton::construct(new_data + position, ANTON_FWD(args)...);
                moved += 1;
                anton::uninitialized_move(_data + position, _data + _size, new_data + moved);

                anton::destruct_n(_data, _size);
                deallocate(_data, _capacity);
                _capacity = new_capacity;
                _data = new_data;
                _size += 1;
            }
        } else {
            // Quick path when position points to end and we have room for one more element.
            anton::construct(_data + _size, ANTON_FWD(args)...);
            _size += 1;
        }

        return _data + position;
    }

    template<typename T>
    template<typename Input_Iterator>
    auto Array<T>::insert(const_iterator position, Input_Iterator first, Input_Iterator last) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert(offset, first, last);
    }

    template<typename T>
    template<typename Input_Iterator>
    auto Array<T>::insert(size_type position, Input_Iterator first, Input_Iterator last) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position >= 0 && position <= _size, u8"index out of bounds");
        }

        // TODO: Distance and actual support for input iterators.
        if(first != last) {
            i64 const new_elems = last - first;
            ANTON_ASSERT(new_elems > 0, "the difference of first and last must be greater than 0");
            if(_size + new_elems <= _capacity && position == _size) {
                // Quick path when position points to end and we have room for new_elems elements.
                anton::uninitialized_copy(first, last, _data + _size);
                _size += new_elems;
            } else {
                if(_size + new_elems <= _capacity) {
                    // total number of elements we want to move
                    i64 const total_elems = _size - position;
                    // when new_elems < total_elems, we have to unititialized_move min(total_elems, new_elems)
                    // and move_backward the rest because the target range will overlap the source range
                    i64 const elems_outside = math::min(total_elems, new_elems);
                    i64 const elems_inside = total_elems - elems_outside;
                    // we move the 'outside' elements to _size unless position + new_elems is greater than _size
                    i64 const target_offset = math::max(position + new_elems, _size);
                    anton::uninitialized_move_n(_data + position + elems_inside, elems_outside, _data + target_offset);
                    anton::move_backward(_data + position, _data + position + elems_inside, _data + position + new_elems + elems_inside);
                    // we always have to destruct at most total_elems and at least new_elems
                    // if we attempt to destruct more than total_elems, we will call destruct on uninitialized memory
                    anton::destruct_n(_data + position, elems_outside);
                    anton::uninitialized_copy(first, last, _data + position);
                    _size += new_elems;
                } else {
                    i64 new_capacity = (_capacity > 0 ? _capacity : ANTON_ARRAY_MIN_ALLOCATION_SIZE);
                    while(new_capacity <= _size + new_elems) {
                        new_capacity *= 2;
                    }

                    T* const new_data = allocate(new_capacity);
                    i64 moved = 0;
                    anton::uninitialized_move(_data, _data + position, new_data);
                    moved = position;
                    anton::uninitialized_copy(first, last, new_data + moved);
                    moved += new_elems;
                    anton::uninitialized_move(_data + position, _data + _size, new_data + moved);

                    anton::destruct_n(_data, _size);
                    deallocate(_data, _capacity);
                    _capacity = new_capacity;
                    _data = new_data;
                    _size += new_elems;
                }
            }
        }

        return _data + position;
    }

    template<typename T>
    auto Array<T>::insert_unsorted(const_iterator position, value_type const& value) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert_unsorted(offset, value);
    }

    template<typename T>
    auto Array<T>::insert_unsorted(const_iterator position, value_type&& value) -> iterator {
        size_type const offset = static_cast<size_type>(position - begin());
        return insert_unsorted(offset, ANTON_MOV(value));
    }

    template<typename T>
    auto Array<T>::insert_unsorted(size_type position, value_type const& value) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position >= 0 && position <= _size, u8"index out of bounds");
        }

        ensure_capacity(_size + 1);
        T* elem_ptr = _data + position;
        if(position == _size) {
            anton::construct(elem_ptr, value);
        } else {
            if constexpr(is_move_constructible<T>) {
                anton::construct(_data + _size, ANTON_MOV(*elem_ptr));
            } else {
                anton::construct(_data + _size, *elem_ptr);
            }
            anton::destruct(elem_ptr);
            anton::construct(elem_ptr, value);
        }

        ++_size;
        return elem_ptr;
    }

    template<typename T>
    auto Array<T>::insert_unsorted(size_type position, value_type&& value) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position >= 0 && position <= _size, u8"index out of bounds");
        }

        ensure_capacity(_size + 1);
        T* elem_ptr = _data + position;
        if(position == _size) {
            anton::construct(elem_ptr, ANTON_MOV(value));
        } else {
            if constexpr(is_move_constructible<T>) {
                anton::construct(_data + _size, ANTON_MOV(*elem_ptr));
            } else {
                anton::construct(_data + _size, *elem_ptr);
            }
            anton::destruct(elem_ptr);
            anton::construct(elem_ptr, ANTON_MOV(value));
        }

        ++_size;
        return elem_ptr;
    }

    template<typename T>
    auto Array<T>::push_back(value_type const& value) -> T& {
        ensure_capacity(_size + 1);
        T* const element = _data + _size;
        anton::construct(element, value);
        ++_size;
        return *element;
    }

    template<typename T>
    auto Array<T>::push_back(value_type&& value) -> T& {
        ensure_capacity(_size + 1);
        T* const element = _data + _size;
        anton::construct(element, ANTON_MOV(value));
        ++_size;
        return *element;
    }

    template<typename T>
    template<typename... Args>
    auto Array<T>::emplace_back(Args&&... args) -> T& {
        ensure_capacity(_size + 1);
        T* const element = _data + _size;
        anton::construct(element, ANTON_FWD(args)...);
        ++_size;
        return *element;
    }

    template<typename T>
    void Array<T>::erase_unsorted(size_type index) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index <= _size && index >= 0, u8"index out of bounds");
        }

        erase_unsorted_unchecked(index);
    }

    template<typename T>
    void Array<T>::erase_unsorted_unchecked(size_type index) {
        T* const element = _data + index;
        T* const last_element = _data + _size - 1;
        if(element != last_element) { // Prevent self assignment
            *element = ANTON_MOV(*last_element);
        }
        anton::destruct(last_element);
        --_size;
    }

    template<typename T>
    auto Array<T>::erase_unsorted(const_iterator iter) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(iter - _data >= 0 && iter - _data <= _size, "iterator out of bounds");
        }

        T* const position = const_cast<T*>(iter);
        T* const last_element = _data + _size - 1;
        if(position != last_element) {
            *position = ANTON_MOV(*last_element);
        }
        anton::destruct(last_element);
        --_size;
        return position;
    }

    //     template <typename T>
    //     auto Array<T>::erase_unsorted(const_iterator first, const_iterator last) -> iterator {
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

    template<typename T>
    auto Array<T>::erase(const_iterator first, const_iterator last) -> iterator {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(first - _data >= 0 && first - _data <= _size, "iterator out of bounds");
            ANTON_FAIL(last - _data >= 0 && last - _data <= _size, "iterator out of bounds");
        }

        if(first != last) {
            iterator pos = anton::move(const_cast<value_type*>(last), end(), const_cast<value_type*>(first));
            anton::destruct(pos, end());
            _size -= last - first;
        }

        return const_cast<value_type*>(first);
    }

    template<typename T>
    void Array<T>::pop_back() {
        ANTON_VERIFY(_size > 0, u8"pop_back called on an empty Array");
        anton::destruct(_data + _size - 1);
        --_size;
    }

    template<typename T>
    void Array<T>::clear() {
        anton::destruct(_data, _data + _size);
        _size = 0;
    }

    template<typename T>
    T* Array<T>::allocate(size_type const size) {
        void* mem = _allocator.allocate(size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
        return static_cast<T*>(mem);
    }

    template<typename T>
    void Array<T>::deallocate(void* mem, size_type const size) {
        _allocator.deallocate(mem, size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
    }
} // namespace anton
