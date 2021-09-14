#include <anton/string.hpp>

#include <anton/assert.hpp>
#include <anton/detail/crt.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/string_utils.hpp>
#include <anton/swap.hpp>
#include <anton/type_traits.hpp>
#include <anton/unicode/common.hpp>

// TODO: Write custom to_string functions
#include <stdio.h> // sprintf

namespace anton {
    constexpr i64 min_allocation_size = 64;

    String String::from_utf32(char32 const* string, i64 const length) {
        i64 const buffer_size = unicode::convert_utf32_to_utf8(string, length, nullptr);
        // null-terminator is present when length == -1 in which case we have to subtract it from the size since
        // String automatically takes care of that
        i64 const str_size = (length == -1 ? buffer_size - 1 : buffer_size);
        String str{anton::reserve, str_size};
        str.force_size(str_size);
        unicode::convert_utf32_to_utf8(string, length, str.data());
        return str;
    }

    String String::from_utf16(char16 const* string, i64 const length) {
        i64 const buffer_size = unicode::convert_utf16_to_utf8(string, length, nullptr);
        i64 const str_size = (length == -1 ? buffer_size - 1 : buffer_size);
        String str{anton::reserve, str_size};
        str.force_size(str_size);
        unicode::convert_utf16_to_utf8(string, length, str.data());
        return str;
    }

    String::String(): _allocator() {}

    String::String(allocator_type const& allocator): _allocator(allocator) {}

    String::String(Reserve_Tag, size_type n): String(anton::reserve, n, allocator_type()) {}

    String::String(Reserve_Tag, size_type n, allocator_type const& allocator): _allocator(allocator) {
        _capacity = math::max(min_allocation_size - 1, n) + 1;
        _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
        zero_memory(_data, _data + _capacity);
    }

    String::String(value_type const* cstr): String(cstr, allocator_type()) {}

    String::String(value_type const* cstr, allocator_type const& allocator): _allocator(allocator) {
        _size = strlen(cstr);
        _capacity = math::max(min_allocation_size - 1, _size) + 1;
        _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
        zero_memory(_data + _size, _data + _capacity);
        copy(cstr, cstr + _size, _data);

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String::String(value_type const* cstr, size_type n): String(cstr, n, allocator_type()) {}

    String::String(value_type const* cstr, size_type n, allocator_type const& allocator): _allocator(allocator) {
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

    String::String(value_type const* first, value_type const* last): String(first, last, allocator_type()) {}

    String::String(value_type const* first, value_type const* last, allocator_type const& allocator) {
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

    String::String(String_View const sv): String(sv, allocator_type()) {}

    String::String(String_View const sv, allocator_type const& allocator): _allocator(allocator) {
        _size = sv.size_bytes();
        if(_size > 0) {
            _capacity = math::max(min_allocation_size - 1, _size) + 1;
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(sv.bytes_begin(), sv.bytes_end(), _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String::String(String const& other): String(other, allocator_type()) {}

    String::String(String const& other, allocator_type const& allocator): _allocator(allocator), _capacity(other._capacity), _size(other._size) {
        if(_capacity > 0) {
            _data = (value_type*)_allocator.allocate(_capacity, alignof(value_type));
            zero_memory(_data + _size, _data + _capacity);
            copy(other._data, other._data + other._size, _data);
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String::String(String&& other): _allocator(ANTON_MOV(other._allocator)), _data(other._data), _capacity(other._capacity), _size(other._size) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    String::String(String&& other, allocator_type const& allocator): _allocator(allocator) {
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

    String::~String() {
        _allocator.deallocate(_data, _capacity, alignof(value_type));
    }

    String& String::operator=(String const& other) {
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

    String& String::operator=(String&& other) {
        swap(*this, other);
        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }

        return *this;
    }

    String& String::operator=(String_View const sv) {
        size_type const new_size = sv.size_bytes();
        if(new_size + 1 > _capacity) {
            size_type new_capacity = min_allocation_size;
            while(new_capacity < new_size + 1) {
                new_capacity *= 2;
            }

            // The String_View can not possibly point to our own memory (self-assignment)
            // because its size is greater than our capacity.
            value_type* const new_data = (value_type*)_allocator.allocate(new_capacity, alignof(value_type));
            zero_memory(new_data + new_size, new_data + new_capacity);
            copy(sv.data(), sv.data() + sv.size_bytes(), new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = new_capacity;
            _size = new_size;
        } else {
            // The String_View may point to our own memory!
            zero_memory(_data + new_size, _data + _capacity);
            copy(sv.data(), sv.data() + sv.size_bytes(), _data);
            _size = new_size;
        }

        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }

        return *this;
    }

    String& String::operator=(value_type const* const cstr) {
        *this = String_View(cstr);
        return *this;
    }

    String::operator String_View() const {
        return {_data, _size};
    }

    String::allocator_type& String::get_allocator() {
        return _allocator;
    }

    String::allocator_type const& String::get_allocator() const {
        return _allocator;
    }

    auto String::data() -> value_type* {
        return _data;
    }

    auto String::data() const -> value_type const* {
        return _data;
    }

    auto String::c_str() const -> value_type const* {
        return _data;
    }

    auto String::bytes() -> UTF8_Bytes {
        return {_data, _data + _size};
    }

    auto String::bytes() const -> UTF8_Const_Bytes {
        return {_data, _data + _size};
    }

    auto String::const_bytes() const -> UTF8_Const_Bytes {
        return {_data, _data + _size};
    }

    auto String::chars() const -> UTF8_Chars {
        return {_data, _data + _size};
    }

    auto String::bytes_begin() -> byte_iterator {
        return _data;
    }

    auto String::bytes_begin() const -> byte_const_iterator {
        return _data;
    }

    auto String::bytes_cbegin() const -> byte_const_iterator {
        return _data;
    }

    auto String::bytes_end() -> byte_iterator {
        return _data + _size;
    }

    auto String::bytes_end() const -> byte_const_iterator {
        return _data + _size;
    }

    auto String::bytes_cend() const -> byte_const_iterator {
        return _data + _size;
    }

    auto String::chars_begin() const -> char_iterator {
        return char_iterator{_data, 0};
    }

    auto String::chars_end() const -> char_iterator {
        return char_iterator{_data + _size, _size};
    }

    auto String::capacity() const -> size_type {
        return _capacity;
    }

    auto String::size_bytes() const -> size_type {
        return _size;
    }

    auto String::size_utf8() const -> size_type {
        return chars_end() - chars_begin();
    }

    void String::ensure_capacity(size_type const requested_capacity) {
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

    void String::ensure_capacity_exact(size_type const requested_capacity) {
        if(requested_capacity > _capacity) {
            value_type* new_data = (value_type*)_allocator.allocate(requested_capacity, alignof(value_type));
            zero_memory(new_data + _size, new_data + requested_capacity);
            move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = requested_capacity;
        }
    }

    void String::force_size(size_type n) {
        _size = n;
    }

    void String::clear() {
        zero_memory(_data, _data + _size);
        _size = 0;
    }

    void String::append(char8 const c) {
        ensure_capacity(_size + 1);
        _data[_size] = c;
        _size += 1;
    }

    void String::append(char32 const c) {
        ensure_capacity(_size + 4);
        i64 const bytes_written = unicode::convert_utf32_to_utf8(&c, 4, _data + _size);
        _size += bytes_written;
    }

    void String::append(String_View str) {
        ensure_capacity(_size + str.size_bytes());
        copy(str.bytes_begin(), str.bytes_end(), _data + _size);
        _size += str.size_bytes();
    }

    inline namespace literals {
        String operator"" _s(char8 const* const literal, u64 const size) {
            return String{literal, (i64)size};
        }
    } // namespace literals

    void swap(String& str1, String& str2) {
        if(str1._allocator == str2._allocator) {
            swap(str1._allocator, str2._allocator);
            swap(str1._data, str2._data);
            swap(str1._capacity, str2._capacity);
            swap(str1._size, str2._size);
        } else {
            // Move operations call swap, which could lead to an endless loop
            String temp{str1};
            str1 = str2;
            str2 = temp;
        }
    }

    bool operator==(String const& lhs, String const& rhs) {
        if(lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        char8 const* lhs_f = lhs.bytes_begin();
        char8 const* lhs_e = lhs.bytes_end();
        char8 const* rhs_f = rhs.bytes_begin();
        char8 const* rhs_e = rhs.bytes_end();
        for(; lhs_f != lhs_e && rhs_f != rhs_e; ++lhs_f, ++rhs_f) {
            if(*lhs_f != *rhs_f) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(String const& lhs, String const& rhs) {
        return !(lhs == rhs);
    }

    String& operator+=(String& lhs, char8 rhs) {
        lhs.append(rhs);
        return lhs;
    }

    String& operator+=(String& lhs, char32 rhs) {
        lhs.append(rhs);
        return lhs;
    }

    String& operator+=(String& lhs, String_View string) {
        lhs.append(string);
        return lhs;
    }

    String operator+(String const& lhs, String const& rhs) {
        String str(anton::reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(String_View lhs, String const& rhs) {
        String str(anton::reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(String const& lhs, String_View rhs) {
        String str(anton::reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(char8 const* lhs, String const& rhs) {
        return String_View(lhs) + rhs;
    }

    String operator+(String const& lhs, char8 const* rhs) {
        return lhs + String_View(rhs);
    }

    String to_string(i32 v) {
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
        return String{buffer + i + !has_sign, buffer + 11};
    }

    String to_string(u32 v) {
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
        return String{buffer + i + 1, buffer + 10};
    }

    String to_string(i64 v) {
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
        return String{buffer + i + !has_sign, buffer + 21};
    }

    String to_string(u64 v) {
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
        return String{buffer + i + 1, buffer + 21};
    }

    String to_string(f32 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.7f", value);
        return String{buffer, written_chars};
    }

    String to_string(f64 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.14f", value);
        return String{buffer, written_chars};
    }

    String to_string(void const* value) {
        char buffer[50] = {};
        usize address = reinterpret_cast<usize>(value);
        i32 written_chars = sprintf(buffer, "0x%016llx", address);
        return String{buffer, written_chars};
    }

    f32 str_to_f32(String const& string) {
        return ::strtof(string.data(), nullptr);
    }

    [[nodiscard]] static bool match(char8 const* iterator, String_View const pattern) {
        char8 const* b = pattern.bytes_begin();
        char8 const* const e = pattern.bytes_end();
        for(; b != e && *iterator == *b; ++iterator, ++b) {}
        return b == e;
    }

    String replace(String_View const string, String_View const pattern, String_View const replacement) {
        // Preallocate memory. We can't estimate how much we will need,
        // so we allocate enough to fit the old string.
        String replaced{reserve, string.size_bytes()};
        char8 const* iterator = string.bytes_begin();
        char8 const* const end = string.bytes_end();
        i64 const pattern_size = pattern.size_bytes();
        if(pattern_size > 0) {
            while(end - iterator >= pattern_size) {
                bool const matched = match(iterator, pattern);
                if(matched) {
                    replaced.append(replacement);
                    iterator += pattern_size;
                } else {
                    // TODO: Assumes there is no encoding validation in append(char8).
                    replaced.append(*iterator);
                    iterator += 1;
                }
            }
        }

        // Append the rest of the string.
        char8* const replaced_end = replaced.data() + replaced.size_bytes();
        i64 const remaining_size = end - iterator;
        i64 const total_size = replaced.size_bytes() + remaining_size;
        replaced.ensure_capacity(total_size);
        replaced.force_size(total_size);
        copy(iterator, end, replaced_end);
        return replaced;
    }
} // namespace anton
