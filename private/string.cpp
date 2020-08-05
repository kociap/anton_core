#include <anton/string.hpp>

#include <anton/assert.hpp>
#include <anton/detail/crt.hpp>
#include <anton/detail/utility_common.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/string_utils.hpp>
#include <anton/type_traits.hpp>
#include <anton/unicode/common.hpp>

// TODO: Replace with format.
#include <stdio.h> // sprintf

namespace anton {
    String String::from_utf32(char32 const* string, i64 const length) {
        i32 const buffer_size = unicode::convert_utf32_to_utf8(string, length, nullptr);
        String str{anton::reserve, buffer_size - 1};
        str.force_size(buffer_size - 1);
        unicode::convert_utf32_to_utf8(string, length, str.data());
        return str;
    }

    String String::from_utf16(char16 const* string, i64 const length) {
        i32 const buffer_size = unicode::convert_utf16_to_utf8(string, length, nullptr);
        String str{anton::reserve, buffer_size - 1};
        str.force_size(buffer_size - 1);
        unicode::convert_utf16_to_utf8(string, length, str.data());
        return str;
    }

    String::String(): String(allocator_type()) {}

    String::String(allocator_type const& allocator): _allocator(allocator) {
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data, 0, _capacity);
    }

    String::String(Reserve_Tag, size_type n): String(anton::reserve, n, allocator_type()) {}

    String::String(Reserve_Tag, size_type n, allocator_type const& allocator): _allocator(allocator) {
        _capacity = math::max(_capacity - 1, n) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data, 0, _capacity);
    }

    String::String(value_type const* str): String(str, allocator_type()) {}

    String::String(value_type const* cstr, allocator_type const& allocator): _allocator(allocator) {
        _size = strlen(cstr);
        if constexpr(ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
        _capacity = math::max(_capacity - 1, _size) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        copy(cstr, cstr + _size, _data);
    }

    String::String(value_type const* cstr, size_type n): String(cstr, n, allocator_type()) {}

    String::String(value_type const* cstr, size_type n, allocator_type const& allocator): _allocator(allocator) {
        _size = n;
        _capacity = math::max(_capacity - 1, n) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        copy(cstr, cstr + n, _data);
    }

    String::String(String_View const sv): String(sv, allocator_type()) {}

    String::String(String_View const sv, allocator_type const& allocator): _allocator(allocator) {
        _size = sv.size_bytes();
        _capacity = math::max(_capacity - 1, _size) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        copy(sv.bytes_begin(), sv.bytes_end(), _data);
    }

    String::String(String const& other): String(other, allocator_type()) {}

    String::String(String const& other, allocator_type const& allocator): _allocator(allocator), _capacity(other._capacity), _size(other._size) {
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        copy(other._data, other._data + other._size, _data);
    }

    String::String(String&& other) noexcept: _allocator(move(other._allocator)), _data(other._data), _capacity(other._capacity), _size(other._size) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
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
            _size = other._size;
            _capacity = other._capacity;
            _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
            memset(_data + _size, 0, _capacity - _size);
            move(other._data, other._data + other._size, _data);
        }
    }

    String::~String() {
        if(_data != nullptr) {
            _allocator.deallocate(_data, _capacity, alignof(value_type));
        }
    }

    String& String::operator=(String const& other) {
        value_type* new_data = reinterpret_cast<value_type*>(_allocator.allocate(other._capacity, alignof(value_type)));
        _allocator.deallocate(_data, _capacity, alignof(value_type));
        _data = new_data;
        _size = other._size;
        _capacity = other._capacity;
        memset(_data + _size, 0, _capacity - _size);
        copy(other._data, other._data + other._size, _data);
        return *this;
    }

    String& String::operator=(String&& other) noexcept {
        swap(*this, other);
        return *this;
    }

    String& String::operator=(String_View const sv) {
        _size = sv.size_bytes();
        size_type const old_capacity = _capacity;
        _capacity = 64;
        while(_capacity < _size + 1) {
            _capacity *= 2;
        }

        value_type* new_data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        _allocator.deallocate(_data, old_capacity, alignof(value_type));
        _data = new_data;

        memset(_data + _size, 0, _capacity - _size);
        copy(sv.data(), sv.data() + sv.size_bytes(), _data);
        return *this;
    }

    String& String::operator=(value_type const* const str) {
        *this = String_View(str);
        return *this;
    }

    // Implicit conversion operator
    String::operator String_View() const {
        return {_data, _size};
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

    void String::reserve(size_type n) {
        ensure_capacity(n);
    }

    void String::reserve_exact(size_type n) {
        ensure_capacity_exact(n);
    }

    void String::force_size(size_type n) {
        _size = n;
    }

    void String::clear() {
        memset(_data, 0, _size);
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
        ensure_capacity(_size + str.size_bytes() + 1);
        copy(str.bytes_begin(), str.bytes_end(), _data + _size);
        _size += str.size_bytes();
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

    String::allocator_type& String::get_allocator() {
        return _allocator;
    }

    String::allocator_type const& String::get_allocator() const {
        return _allocator;
    }

    void String::ensure_capacity(size_type requested_capacity) {
        if(requested_capacity >= _capacity) {
            size_type new_capacity = _capacity;
            while(new_capacity <= requested_capacity) {
                new_capacity *= 2;
            }

            value_type* new_data = static_cast<value_type*>(_allocator.allocate(new_capacity, alignof(value_type)));
            memset(new_data + _size, 0, new_capacity - _size);
            move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = new_capacity;
        }
    }

    void String::ensure_capacity_exact(size_type requested_capacity) {
        if(requested_capacity > _capacity) {
            value_type* new_data = static_cast<value_type*>(_allocator.allocate(requested_capacity, alignof(value_type)));
            memset(new_data + _size, 0, requested_capacity - _size);
            move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = requested_capacity;
        }
    }

    String& operator+=(String& str, char8 c) {
        str.append(c);
        return str;
    }

    String& operator+=(String& str, char32 c) {
        str.append(c);
        return str;
    }

    String& operator+=(String& str, String_View sv) {
        str.append(sv);
        return str;
    }

    void swap(String& str1, String& str2) {
        swap(str1._allocator, str2._allocator);
        swap(str1._data, str2._data);
        swap(str1._capacity, str2._capacity);
        swap(str1._size, str2._size);
    }

    bool operator==(String const& lhs, String const& rhs) {
        if(lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data());
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

    String operator+(char const* lhs, String const& rhs) {
        return String_View(lhs) + rhs;
    }

    String operator+(String const& lhs, char const* rhs) {
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
        i32 len = 12 - i - !has_sign;
        // TODO: Implement (ptr, ptr) string ctor
        // return {buffer + i, buffer + 12};
        return {buffer + i + !has_sign, len - 1};
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
        i32 len = 12 - i - 1;
        // TODO: Implement (ptr, ptr) string ctor
        // return {buffer + i + 1, buffer + 11};
        return {buffer + i + 1, len - 1};
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
        i32 len = 22 - i - !has_sign;
        // TODO: Implement (ptr, ptr) string ctor
        // return {buffer + i, buffer + 22};
        return {buffer + i + !has_sign, len - 1};
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
        i64 len = 21 - i - 1;
        // TODO: Implement (ptr, ptr) string ctor
        // return {buffer + i + 1, buffer + 22};
        return {buffer + i + 1, len - 1};
    }

    String to_string(f32 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.7f", value);
        return {buffer, written_chars};
    }

    String to_string(f64 value) {
        char buffer[50] = {};
        i32 written_chars = sprintf(buffer, "%.14f", value);
        return {buffer, written_chars};
    }

    String to_string(void* value) {
        char buffer[50] = {};
        usize address = reinterpret_cast<usize>(value);
        i32 written_chars = sprintf(buffer, "0x%016llx", address);
        return {buffer, written_chars};
    }

    f32 str_to_f32(String const& string) {
        return ::strtof(string.data(), nullptr);
    }
} // namespace anton
