#include "anton/allocator.hpp"
#include <anton/string7.hpp>

#include <anton/assert.hpp>
#include <anton/detail/crt.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/string_utils.hpp>
#include <anton/swap.hpp>
#include <anton/type_traits.hpp>
#include <anton/unicode/common.hpp>

// TODO: Write custom to_string7 functions
#include <stdio.h> // sprintf

namespace anton {
    constexpr i64 min_allocation_size = 64;

    String7::String7(): _allocator() {}

    String7::String7(allocator_type const& allocator): _allocator(allocator) {}

    String7::String7(Reserve_Tag, size_type n): String7(anton::reserve, n, allocator_type()) {}

    String7::String7(Reserve_Tag, size_type n, allocator_type const& allocator): _allocator(allocator) {
        _capacity = math::max(min_allocation_size - 1, n) + 1;
        _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
        zero_memory(_data, _data + _capacity);
    }

    String7::String7(value_type const* cstr): String7(cstr, allocator_type()) {}

    String7::String7(value_type const* cstr, allocator_type const& allocator): _allocator(allocator) {
        _size = strlen(cstr);
        _capacity = math::max(min_allocation_size - 1, _size) + 1;
        _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
        zero_memory(_data + _size, _data + _capacity);
        copy(cstr, cstr + _size, _data);

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(value_type const* cstr, size_type n): String7(cstr, n, allocator_type()) {}

    String7::String7(value_type const* cstr, size_type n, allocator_type const& allocator): _allocator(allocator) {
        if(n > 0) {
            _size = n;
            _capacity = math::max(min_allocation_size - 1, _size) + 1;
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(cstr, cstr + n, _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(value_type const* first, value_type const* last): String7(first, last, allocator_type()) {}

    String7::String7(value_type const* first, value_type const* last, allocator_type const& allocator) {
        _size = last - first;
        if(_size > 0) {
            _capacity = math::max(min_allocation_size - 1, _size) + 1;
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(first, last, _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(String7_View const sv): String7(sv, allocator_type()) {}

    String7::String7(String7_View const sv, allocator_type const& allocator): _allocator(allocator) {
        _size = sv.size();
        if(_size > 0) {
            _capacity = math::max(min_allocation_size - 1, _size) + 1;
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(sv.begin(), sv.end(), _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(String7 const& other): String7(other, allocator_type()) {}

    String7::String7(String7 const& other, allocator_type const& allocator): _allocator(allocator), _capacity(other._capacity), _size(other._size) {
        if(_capacity > 0) {
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(other._data, other._data + other._size, _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(String7&& other): _allocator(ANTON_MOV(other._allocator)), _data(other._data), _capacity(other._capacity), _size(other._size) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::String7(String7&& other, allocator_type const& allocator): _allocator(allocator) {
        if(_allocator == other._allocator) {
            _data = other._data;
            other._data = nullptr;
            _capacity = other._capacity;
            other._capacity = 0;
            _size = other._size;
            other._size = 0;
        } else {
            if(other._capacity > 0) {
                _size = other._size;
                _capacity = other._capacity;
                _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
                zero_memory(_data + _size, _data + _capacity);
                move(other._data, other._data + other._size, _data);
            }
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String7::~String7() {
        _allocator.deallocate(_data, _capacity, alignof(value_type));
    }

    String7& String7::operator=(String7 const& other) {
        if(other._size + 1 > _capacity) {
            value_type* const new_data = (value_type*)_allocator.allocate(other._capacity, alignof(value_type));
            zero_memory(new_data + other._size, new_data + other._capacity);
            copy(other._data, other._data + other._size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = other._capacity;
            _size = other._size;
        } else {
            zero_memory(_data + other._size, _data + _capacity);
            copy(other._data, other._data + other._size, _data);
            _size = other._size;
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }

        return *this;
    }

    String7& String7::operator=(String7&& other) {
        swap(*this, other);
        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }

        return *this;
    }

    String7& String7::operator=(String7_View const sv) {
        size_type const new_size = sv.size();
        if(new_size + 1 > _capacity) {
            size_type new_capacity = min_allocation_size;
            while(new_capacity < new_size + 1) {
                new_capacity *= 2;
            }

            // The String7_View can not possibly point to our own memory (self-assignment)
            // because its size is greater than our capacity.
            value_type* const new_data = (value_type*)_allocator.allocate(new_capacity, alignof(value_type));
            zero_memory(new_data + new_size, new_data + new_capacity);
            copy(sv.data(), sv.data() + sv.size(), new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = new_capacity;
            _size = new_size;
        } else {
            // The String7_View may point to our own memory!
            zero_memory(_data + new_size, _data + _capacity);
            copy(sv.data(), sv.data() + sv.size(), _data);
            _size = new_size;
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }

        return *this;
    }

    String7& String7::operator=(value_type const* const cstr) {
        *this = String7_View(cstr);
        return *this;
    }

    String7::operator String7_View() const {
        return {_data, _size};
    }

    String7::allocator_type& String7::get_allocator() {
        return _allocator;
    }

    String7::allocator_type const& String7::get_allocator() const {
        return _allocator;
    }

    auto String7::operator[](size_type const index) const -> value_type {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(index < _size && index >= 0, u8"index out of bounds");
        }

        return _data[index];
    }

    auto String7::data() -> value_type* {
        return _data;
    }

    auto String7::data() const -> value_type const* {
        return _data;
    }

    auto String7::c_str() const -> value_type const* {
        return _data;
    }

    auto String7::begin() -> iterator {
        return _data;
    }

    auto String7::begin() const -> const_iterator {
        return _data;
    }

    auto String7::cbegin() const -> const_iterator {
        return _data;
    }

    auto String7::end() -> iterator {
        return _data + _size;
    }

    auto String7::end() const -> const_iterator {
        return _data + _size;
    }

    auto String7::cend() const -> const_iterator {
        return _data + _size;
    }

    auto String7::capacity() const -> size_type {
        return _capacity;
    }

    auto String7::size() const -> size_type {
        return _size;
    }

    void String7::ensure_capacity(size_type const requested_capacity) {
        if(requested_capacity >= _capacity) {
            size_type new_capacity = (_capacity > 0 ? _capacity : min_allocation_size);
            while(new_capacity <= requested_capacity) {
                new_capacity *= 2;
            }

            value_type* new_data = (value_type*)_allocator.allocate(new_capacity, alignof(value_type));
            zero_memory(new_data + _size, new_data + new_capacity);
            move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = new_capacity;
        }
    }

    void String7::ensure_capacity_exact(size_type const requested_capacity) {
        if(requested_capacity > _capacity) {
            value_type* new_data = (value_type*)_allocator.allocate(requested_capacity, alignof(value_type));
            zero_memory(new_data + _size, new_data + requested_capacity);
            move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = requested_capacity;
        }
    }

    void String7::force_size(size_type n) {
        _size = n;
    }

    void String7::clear() {
        zero_memory(_data, _data + _size);
        _size = 0;
    }

    void String7::append(char8 const c) {
        ensure_capacity(_size + 1);
        _data[_size] = c;
        _size += 1;
    }

    void String7::append(String7_View str) {
        ensure_capacity(_size + str.size());
        copy(str.begin(), str.end(), _data + _size);
        _size += str.size();
    }

    inline namespace literals {
        String7 operator""_s7(char8 const* const literal, u64 const size) {
            return String7{literal, (i64)size};
        }
    } // namespace literals

    void swap(String7& str1, String7& str2) {
        if(str1._allocator == str2._allocator) {
            swap(str1._allocator, str2._allocator);
            swap(str1._data, str2._data);
            swap(str1._capacity, str2._capacity);
            swap(str1._size, str2._size);
        } else {
            // Move operations call swap, which could lead to an endless loop
            String7 temp{str1};
            str1 = str2;
            str2 = temp;
        }
    }

    bool operator==(String7 const& lhs, String7 const& rhs) {
        if(lhs.size() != rhs.size()) {
            return false;
        }

        char8 const* lhs_f = lhs.begin();
        char8 const* lhs_e = lhs.end();
        char8 const* rhs_f = rhs.begin();
        char8 const* rhs_e = rhs.end();
        for(; lhs_f != lhs_e && rhs_f != rhs_e; ++lhs_f, ++rhs_f) {
            if(*lhs_f != *rhs_f) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(String7 const& lhs, String7 const& rhs) {
        return !(lhs == rhs);
    }

    String7& operator+=(String7& lhs, char8 rhs) {
        lhs.append(rhs);
        return lhs;
    }

    String7& operator+=(String7& lhs, String7_View string) {
        lhs.append(string);
        return lhs;
    }

    String7 operator+(String7 const& lhs, String7 const& rhs) {
        String7 str(anton::reserve, lhs.size() + rhs.size());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String7 operator+(String7_View lhs, String7 const& rhs) {
        String7 str(anton::reserve, lhs.size() + rhs.size());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String7 operator+(String7 const& lhs, String7_View rhs) {
        String7 str(anton::reserve, lhs.size() + rhs.size());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String7 operator+(char8 const* lhs, String7 const& rhs) {
        return String7_View(lhs) + rhs;
    }

    String7 operator+(String7 const& lhs, char8 const* rhs) {
        return lhs + String7_View(rhs);
    }

    String7 to_string7(i32 v) {
        // We don't need null-terminator or initialized elements.
        char buffer[11];
        bool has_sign = false;
        if(v < 0) {
            has_sign = true;
            v = -v;
        }

        i32 i = 10;
        do {
            buffer[i] = (v % 10) | '0';
            --i;
            v /= 10;
        } while(v > 0);
        buffer[i] = '-';
        return String7{buffer + i + !has_sign, buffer + 11};
    }

    String7 to_string7(u32 v) {
        // We don't need null-terminator or initialized elements.
        char buffer[10];
        i32 i = 9;
        do {
            buffer[i] = (v % 10) | '0';
            --i;
            v /= 10;
        } while(v > 0);
        // We add 1 to readjust after the last decrement in the loop
        // that takes us to a position before the first digit.
        return String7{buffer + i + 1, buffer + 10};
    }

    String7 to_string7(i64 v) {
        // We don't need null-terminator or initialized elements.
        char buffer[21];
        bool has_sign = false;
        if(v < 0) {
            has_sign = true;
            v = -v;
        }

        i32 i = 20;
        do {
            buffer[i] = (v % 10) | '0';
            --i;
            v /= 10;
        } while(v > 0);
        buffer[i] = '-';
        return String7{buffer + i + !has_sign, buffer + 21};
    }

    String7 to_string7(u64 v) {
        // We don't need null-terminator or initialized elements.
        char buffer[21];
        i64 i = 20;
        do {
            buffer[i] = (v % 10) | '0';
            --i;
            v /= 10;
        } while(v > 0);
        // We add 1 to readjust after the last decrement in the loop
        // that takes us to a position before the first digit.
        return String7{buffer + i + 1, buffer + 21};
    }

    String7 to_string7(f32 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.7f", value);
        return String7{buffer, written_chars};
    }

    String7 to_string7(f64 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.14f", value);
        return String7{buffer, written_chars};
    }

    String7 to_string7(void const* value) {
        char buffer[50] = {};
        usize address = reinterpret_cast<usize>(value);
        i32 written_chars = sprintf(buffer, "0x%016llx", address);
        return String7{buffer, written_chars};
    }

    f32 str_to_f32(String7 const& string) {
        return ::strtof(string.data(), nullptr);
    }

    String7 to_lower(Memory_Allocator* const allocator, String7_View const string) {
        String7 result{string, allocator};
        for(char8& c: result) {
            if(c <= 'Z' && c >= 'A') {
                c = c | 0x20;
            }
        }
        return result;
    }

    String7 to_lower(String7_View string) {
        return to_lower(get_default_allocator(), string);
    }

    String7 to_upper(Memory_Allocator* const allocator, String7_View const string) {
        String7 result{string, allocator};
        for(char8& c: result) {
            if(c <= 'z' && c >= 'a') {
                c = c & 0xDF;
            }
        }
        return result;
    }

    String7 to_upper(String7_View string) {
        return to_lower(get_default_allocator(), string);
    }
} // namespace anton
