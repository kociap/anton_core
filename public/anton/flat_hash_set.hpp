#pragma once

#include <anton/allocator.hpp>
#include <anton/assert.hpp>
#include <anton/functors.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>
#include <anton/types.hpp>
#include <anton/utility.hpp>

namespace anton {
    // Stores both keys and values in the main array, which minimizes memory indirections.
    // It doesn't provide pointer stability and moves data on rehashing.
    // TODO: Implement state sentinel for iterators.
    // TODO: Add launder.
    //
    template<typename Key, typename Hash = Default_Hash<Key>, typename Key_Equal = Equal_Compare<Key>>
    class Flat_Hash_Set {
    private:
        enum class State : u8 {
            empty = 0,
            active,
            deleted,
            sentinel,
        };

        using Slot = Key;

        template<typename _Key, typename _Hash, typename _Key_Equal, typename = void>
        struct Transparent_Key {
            template<typename>
            using type = _Key const&;
        };

        template<typename _Key, typename _Hash, typename _Key_Equal>
        struct Transparent_Key<_Key, _Hash, _Key_Equal, enable_if<is_transparent<_Hash> && is_transparent<_Key_Equal>>> {
            template<typename Key_Type>
            using type = Key_Type const&;
        };

        template<typename T>
        using transparent_key = typename Transparent_Key<Key, Hash, Key_Equal>::template type<T>;

    public:
        using value_type = Key const;
        using allocator_type = Polymorphic_Allocator;
        using hasher = Hash;
        using key_equal = Key_Equal;

        class const_iterator {
        public:
            using value_type = Key const;
            using difference_type = isize;
            using iterator_category = Bidirectional_Input_Tag;

            const_iterator() = delete;
            const_iterator(const_iterator const&) = default;
            const_iterator(const_iterator&&) = default;
            ~const_iterator() = default;
            const_iterator& operator=(const_iterator const&) = default;
            const_iterator& operator=(const_iterator&&) = default;

            const_iterator& operator++() {
                _slots += 1;
                _states += 1;
                while(*_states == State::empty || *_states == State::deleted) {
                    _slots += 1;
                    _states += 1;
                }
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator iter = *this;
                ++(*this);
                return iter;
            }

            const_iterator& operator--() {
                _slots -= 1;
                _states -= 1;
                while(*_states == State::empty || *_states == State::deleted) {
                    _slots -= 1;
                    _states -= 1;
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator iter = *this;
                --(*this);
                return iter;
            }

            [[nodiscard]] value_type* operator->() const {
                if constexpr(ANTON_ITERATOR_DEBUG) {
                    ANTON_FAIL(*_states == State::active, "Dereferencing invalid Flat_Hash_Set iterator.");
                }
                return _slots;
            }

            [[nodiscard]] value_type& operator*() const {
                if constexpr(ANTON_ITERATOR_DEBUG) {
                    ANTON_FAIL(*_states == State::active, "Dereferencing invalid Flat_Hash_Set iterator.");
                }
                return *_slots;
            }

            [[nodiscard]] bool operator==(const_iterator const& b) const {
                return _slots == b._slots;
            }

            [[nodiscard]] bool operator!=(const_iterator const& b) const {
                return _slots != b._slots;
            }

        private:
            friend class Flat_Hash_Set;
            friend class iterator;

            Slot const* _slots;
            State const* _states;

            const_iterator(Slot const* slots, State const* states): _slots(slots), _states(states) {}
        };

        using iterator = const_iterator;

        Flat_Hash_Set(allocator_type const& = allocator_type(), hasher const& = hasher(), key_equal const& = key_equal());
        Flat_Hash_Set(Reserve_Tag, i64 size, allocator_type const& = allocator_type(), hasher const& = hasher(), key_equal const& = key_equal());
        Flat_Hash_Set(Flat_Hash_Set const&, allocator_type const& = allocator_type());
        Flat_Hash_Set(Flat_Hash_Set&&) noexcept;
        Flat_Hash_Set& operator=(Flat_Hash_Set const&);
        Flat_Hash_Set& operator=(Flat_Hash_Set&&) noexcept;
        ~Flat_Hash_Set();

        [[nodiscard]] iterator begin() {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] const_iterator begin() const {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return const_iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] const_iterator cbegin() {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return const_iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] iterator end() {
            return iterator(_slots + _capacity, _states + _capacity);
        }

        [[nodiscard]] const_iterator end() const {
            return const_iterator(_slots + _capacity, _states + _capacity);
        }

        [[nodiscard]] const_iterator cend() {
            return const_iterator(_slots + _capacity, _states + _capacity);
        }

        template<typename K = void>
        [[nodiscard]] iterator find(transparent_key<K> key) {
            if(_capacity) {
                u64 const h = _hasher(key);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::active && _key_equal(key, _slots[index])) {
                        return iterator(_slots + index, _states + index);
                    } else if(state == State::empty) {
                        return end();
                    }
                    index = (index + 1) % _capacity;
                }
            }
            return end();
        }

        template<typename K = void>
        [[nodiscard]] const_iterator find(transparent_key<K> key) const {
            if(_capacity) {
                u64 const h = _hasher(key);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::active && _key_equal(key, _slots[index])) {
                        return iterator(_slots + index, _states + index);
                    } else if(state == State::empty) {
                        return end();
                    }
                    index = (index + 1) % _capacity;
                }
            }
            return end();
        }

        // find_or_emplace
        // Finds the entry with given key or default constructs one if it doesn't exist.
        //
        template<typename... Args>
        [[nodiscard]] iterator find_or_emplace(Key const& key);

        // emplace
        // Overwrites the value if it already exists.
        //
        template<typename Key_Type>
        iterator emplace(Key_Type&& key);

        void erase(const_iterator position);
        void clear();

        // ensure_capacity
        // Resizes and rehashes the hash map if c elements wouldn't fit into the hash map.
        //
        void ensure_capacity(i64 c);

        void rehash();

        [[nodiscard]] i64 capacity() const {
            return _capacity;
        }

        [[nodiscard]] i64 size() const {
            return _size;
        }

        [[nodiscard]] allocator_type& get_allocator() {
            return _allocator;
        }

        [[nodiscard]] allocator_type const& get_allocator() const {
            return _allocator;
        }

        [[nodiscard]] f32 load_factor() const {
            return (f32)(_capacity - _empty_slots_left) / (f32)_capacity;
        }

        [[nodiscard]] f32 max_load_factor() const {
            return 0.75f;
        }

    private:
        // TODO: Compressed tuple.
        allocator_type _allocator;
        hasher _hasher;
        key_equal _key_equal;
        State* _states = nullptr;
        Slot* _slots = nullptr;
        i64 _capacity = 0;
        i64 _size = 0;
        i64 _empty_slots_left = 0;

        static State* empty_initial_states();
        i64 find_non_active(u64 hash, i64 capacity, State* states);
    };
} // namespace anton

namespace anton {
    template<typename Key, typename Hash, typename Key_Compare>
    Flat_Hash_Set<Key, Hash, Key_Compare>::Flat_Hash_Set(allocator_type const& alloc, hasher const& h, key_equal const& eq)
        : _allocator(alloc), _hasher(h), _key_equal(eq), _states(empty_initial_states()) {}

    template<typename Key, typename Hash, typename Key_Compare>
    Flat_Hash_Set<Key, Hash, Key_Compare>::Flat_Hash_Set(Reserve_Tag, i64 size, allocator_type const& alloc, hasher const& h, key_equal const& eq)
        : _allocator(alloc), _hasher(h), _key_equal(eq) {
        ensure_capacity(size);
    }

    template<typename Key, typename Hash, typename Key_Compare>
    Flat_Hash_Set<Key, Hash, Key_Compare>::Flat_Hash_Set(Flat_Hash_Set const& other, allocator_type const& alloc)
        : _allocator(alloc), _hasher(other._hasher), _key_equal(other._key_equal), _capacity(other._capacity), _size(other._size),
          _empty_slots_left(other._empty_slots_left) {
        if(_capacity) {
            _states = (State*)_allocator.allocate((_capacity + 16 + 1) * sizeof(State), 16);
            memcpy(_states, other._states - 16, (_capacity + 16 + 1) * sizeof(State));
            _states += 16;
            _slots = (Slot*)_allocator.allocate(_capacity * sizeof(Slot), alignof(Slot));
            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    construct(_slots + i, other._slots[i]);
                }
            }
        } else {
            _states = empty_initial_states();
        }
    }

    template<typename Key, typename Hash, typename Key_Compare>
    Flat_Hash_Set<Key, Hash, Key_Compare>::Flat_Hash_Set(Flat_Hash_Set&& other) noexcept
        : _allocator(), _hasher(), _key_equal(), _states(other._states), _slots(other._slots), _capacity(other._capacity), _size(other._size),
          _empty_slots_left(other._empty_slots_left) {
        other._slots = nullptr;
        other._states = nullptr;
        other._capacity = 0;
        other._size = 0;
        other._empty_slots_left = 0;
    }

    template<typename Key, typename Hash, typename Key_Compare>
    auto Flat_Hash_Set<Key, Hash, Key_Compare>::operator=(Flat_Hash_Set const& other) -> Flat_Hash_Set& {
        // TODO: Should it copy the allocator, hasher or key_equal?
        // TODO: Assumes no exceptions.
        // TODO: Turn off the fucking exceptions.
        if(_capacity) {
            _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
            _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
        }

        if(other._capacity) {
            _capacity = other._capacity;
            _size = other._size;
            _empty_slots_left = other._empty_slots_left;
            _states = (State*)_allocator.allocate((_capacity + 16 + 1) * sizeof(State), 16);
            memcpy(_states, other._states - 16, (_capacity + 16 + 1) * sizeof(State));
            _states += 16;
            _slots = (Slot*)_allocator.allocate(_capacity * sizeof(Slot), alignof(Slot));
            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    construct(_slots + i, other._slots[i]);
                }
            }
        } else {
            _states = empty_initial_states();
        }
        return *this;
    }

    template<typename Key, typename Hash, typename Key_Compare>
    auto Flat_Hash_Set<Key, Hash, Key_Compare>::operator=(Flat_Hash_Set&& other) noexcept -> Flat_Hash_Set& {
        swap(_slots, other._slots);
        swap(_states, other._states);
        swap(_capacity, other._capacity);
        swap(_size, other._size);
        swap(_hasher, other._hasher);
        swap(_allocator, other._allocator);
        swap(_key_equal, other._key_equal);
        swap(_empty_slots_left, other._empty_slots_left);
        return other;
    }

    template<typename Key, typename Hash, typename Key_Compare>
    Flat_Hash_Set<Key, Hash, Key_Compare>::~Flat_Hash_Set() {
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::active) {
                destruct(_slots + i);
            }
        }

        if(_capacity) {
            _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
            _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
        }
    }

    template<typename Key, typename Hash, typename Key_Compare>
    template<typename... Args>
    auto Flat_Hash_Set<Key, Hash, Key_Compare>::find_or_emplace(Key const& key) -> iterator {
        // emplace does the exact same thing as find_or_emplace. We keep find_or_emplace for api consistency
        // with other hash-based continers.
        return emplace(key);
    }

    template<typename Key, typename Hash, typename Key_Compare>
    template<typename Key_Type>
    auto Flat_Hash_Set<Key, Hash, Key_Compare>::emplace(Key_Type&& key) -> iterator {
        ensure_capacity(_size + 1);
        u64 const h = _hasher(key);
        i64 index = h % _capacity;
        while(true) {
            State const state = _states[index];
            if(state != State::active) {
                _states[index] = State::active;
                construct(_slots + index, forward<Key_Type>(key));
                _size += 1;
                _empty_slots_left -= 1;
                return iterator(_slots + index, _states + index);
            } else {
                if(_key_equal(key, _slots[index])) {
                    return iterator(_slots + index, _states + index);
                }
                index = (index + 1) % _capacity;
            }
        }
    }

    template<typename Key, typename Hash, typename Key_Compare>
    void Flat_Hash_Set<Key, Hash, Key_Compare>::erase(const_iterator pos) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(pos._states >= _states && pos._states < _states + _capacity,
                       "Flat_Hash_Set::erase(const_iterator): Attepmting to erase an iterator outside the container.");
            ANTON_FAIL(*pos._states == State::active,
                       "Flat_Hash_Set::erase(const_iterator): Attempting to erase an iterator that doesn't point to a valid object.");
        }

        *const_cast<State*>(pos._states) = State::deleted;
        destruct(pos._slots);
        _size -= 1;
    }

    template<typename Key, typename Hash, typename Key_Compare>
    void Flat_Hash_Set<Key, Hash, Key_Compare>::clear() {
        State* i = _states;
        State* const end = _states + _capacity;
        Slot* j = _slots;
        for(; i != end; ++i, ++j) {
            if(*i == State::active) {
                destruct(j);
            }
            *i = State::empty;
        }
    }

    template<typename Key, typename Hash, typename Key_Compare>
    void Flat_Hash_Set<Key, Hash, Key_Compare>::ensure_capacity(i64 const c) {
        i64 const new_elements_count = math::max(c - _size, (i64)0);
        i64 const removed_elements_count = _capacity - _empty_slots_left - _size;
        if(_capacity != 0 && removed_elements_count >= new_elements_count) {
            rehash();
        } else {
            i64 const required_slots = _size + new_elements_count;
            i64 const required_capacity = (i64)math::ceil((f32)required_slots / max_load_factor());
            i64 new_capacity = _capacity != 0 ? _capacity : 64;
            while(new_capacity < required_capacity) {
                new_capacity *= 2;
            }

            if(new_capacity == _capacity) {
                return;
            }

            State* new_states = nullptr;
            Slot* new_slots = nullptr;
            try {
                new_states = (State*)_allocator.allocate((new_capacity + 16 + 1) * sizeof(State), 16);
                new_states += 16;
                new_states[-1] = State::sentinel;
                memset(new_states, (u8)State::empty, new_capacity);
                new_states[new_capacity] = State::sentinel;
                new_slots = (Slot*)_allocator.allocate(new_capacity * sizeof(Slot), alignof(Slot));
            } catch(...) {
                // TODO: Cleanup or whatever.
                throw;
            }

            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    u64 const h = _hasher(_slots[i]);
                    i64 const index = find_non_active(h, new_capacity, new_states);
                    construct(new_slots + index, move(_slots[i]));
                    destruct(_slots + i);
                    new_states[index] = State::active;
                }
            }

            if(_capacity) {
                _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
                _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
            }
            _states = new_states;
            _slots = new_slots;
            _capacity = new_capacity;
        }
    }

    template<typename Key, typename Hash, typename Key_Compare>
    void Flat_Hash_Set<Key, Hash, Key_Compare>::rehash() {
        // Convert deleted to empty and active to deleted.
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::deleted) {
                _states[i] = State::empty;
            } else if(_states[i] == State::active) {
                _states[i] = State::deleted;
            }
        }

        // Rehash all deleted.
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::deleted) {
                Slot& slot = _slots[i];
                u64 const h = _hasher(slot);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::empty) {
                        _states[index] = State::active;
                        construct(_slots + index, move(_slots[i]));
                        break;
                    } else if(state == State::deleted) {
                        _states[index] = State::active;
                        swap(slot, _slots[index]);
                        break;
                    }
                    index = (index + 1) % _capacity;
                }
            }
        }

        _empty_slots_left = _capacity - _size;
    }

    template<typename Key, typename Hash, typename Key_Compare>
    auto Flat_Hash_Set<Key, Hash, Key_Compare>::empty_initial_states() -> State* {
        // constexpr to force the values to be allocated at compiletime.
        // TODO: I have no idea what I'm doing.
        // TODO: Remind myself at one point why I'm aligning stuff.
        alignas(16) static constexpr State empty_state[] = {
            State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel,
            State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel,
        };
        return const_cast<State*>(empty_state);
    }

    template<typename Key, typename Hash, typename Key_Compare>
    i64 Flat_Hash_Set<Key, Hash, Key_Compare>::find_non_active(u64 h, i64 cap, State* states) {
        i64 index = h % cap;
        while(true) {
            State const state = states[index];
            if(state != State::active) {
                break;
            } else {
                index = (index + 1) % cap;
            }
        }
        return index;
    }
} // namespace anton
