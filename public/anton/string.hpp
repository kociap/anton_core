#pragma once

#include <anton/allocator.hpp>
#include <anton/functors.hpp>
#include <anton/iterators.hpp>
#include <anton/string_view.hpp>
#include <anton/tags.hpp>

namespace anton {
    // String
    // UTF-8 encoded string
    //
    // Does not provide begin/end functions, but instead exposes 2 functions - bytes and chars.
    // Both return proxy classes which have begin/end functions that return iterators over
    // bytes and code points respectively.
    //
    // Notes:
    // operator[] is not implemented because UTF-8 doesn't allow us to index in constant-time.
    //
    // TODO: SSO
    // TODO: Grapheme Clusters
    //
    struct String {
    public:
        using value_type = char8;
        using size_type = i64;
        using difference_type = isize;
        using allocator_type = Polymorphic_Allocator;
        using byte_iterator = char8*;
        using byte_const_iterator = char8*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        // from_utf32
        // Construct String from UTF-32 encoded string.
        //
        // length is the number of bytes to be used to construct String. If length is -1,
        // the function will convert all characters up until and including the null-terminator.
        //
        [[nodiscard]] static String from_utf32(char32 const* string, i64 length = -1);

        // from_utf16
        // Construct String from UTF-16 encoded string.
        //
        // length is the number of bytes to be used to construct String. If length is -1,
        // the function will convert all characters up until and including the null-terminator.
        //
        [[nodiscard]] static String from_utf16(char16 const* string, i64 length = -1);

    public:
        String();
        explicit String(allocator_type const& allocator);
        // Reserve space to fit a string of length n and null-terminator.
        explicit String(Reserve_Tag, size_type n);
        // Reserve space to fit a string of length n and null-terminator.
        explicit String(Reserve_Tag, size_type n, allocator_type const& allocator);
        // Constructs String from null-terminated UTF-8 string
        explicit String(value_type const* string);
        // Constructs String from null-terminated UTF-8 string
        explicit String(value_type const* string, allocator_type const& allocator);
        // Constructs String from a string of size n
        explicit String(value_type const* string, size_type n);
        // Constructs String from a string of size n
        explicit String(value_type const* string, size_type n, allocator_type const& allocator);
        // Constructs String from the range [first, last[
        explicit String(value_type const* first, value_type const* last);
        // Constructs String from the range [first, last[
        explicit String(value_type const* first, value_type const* last, allocator_type const& allocator);
        explicit String(String_View string);
        explicit String(String_View string, allocator_type const& allocator);
        // Copies the allocator
        String(String const& other);
        String(String const& other, allocator_type const& allocator);
        // Moves the allocator
        String(String&& other);
        String(String&& other, allocator_type const& allocator);
        ~String();

        // Does not copy the allocator
        String& operator=(String const& other);
        // Does not move the allocator
        String& operator=(String&& other);
        String& operator=(String_View string);
        String& operator=(value_type const* cstr);

    public:
        // Implicit conversion operator
        [[nodiscard]] operator String_View() const;

    public:
        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        [[nodiscard]] value_type* data();
        [[nodiscard]] value_type const* data() const;
        [[nodiscard]] value_type const* c_str() const;

        [[nodiscard]] UTF8_Bytes bytes();
        [[nodiscard]] UTF8_Const_Bytes bytes() const;
        [[nodiscard]] UTF8_Const_Bytes const_bytes() const;
        [[nodiscard]] UTF8_Chars chars() const;

        [[nodiscard]] byte_iterator bytes_begin();
        [[nodiscard]] byte_const_iterator bytes_begin() const;
        [[nodiscard]] byte_const_iterator bytes_cbegin() const;

        [[nodiscard]] byte_iterator bytes_end();
        [[nodiscard]] byte_const_iterator bytes_end() const;
        [[nodiscard]] byte_const_iterator bytes_cend() const;

        // Always const
        [[nodiscard]] char_iterator chars_begin() const;
        // Always const
        [[nodiscard]] char_iterator chars_end() const;

        // Capacity of the string in bytes.
        [[nodiscard]] size_type capacity() const;
        // Size of the string in bytes.
        [[nodiscard]] size_type size_bytes() const;
        // Counts the number of Unicode code points.
        // This is a linear-time operation.
        [[nodiscard]] size_type size_utf8() const;

        // ensure_capacity
        // Allocates at least requested_capacity + 1 (for null-terminator) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void ensure_capacity(size_type requested_capacity);

        // ensure_capacity_exact
        // Allocates exactly requested_capacity (null-terminator is not accounted for) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void ensure_capacity_exact(size_type requested_capacity);

        // force_size
        // Changes the size of the string to n. Useful in situations when the user
        // writes to the string via external means.
        //
        void force_size(size_type n);

        void clear();
        void append(char8 c);
        void append(char32 c);
        void append(String_View string);
        // template <typename Input_Iterator>
        // iterator insert(const_iterator pos, Input_Iterator first, Input_Iterator last);

        friend void swap(String& lhs, String& rhs);

    private:
        allocator_type _allocator;
        value_type* _data = nullptr;
        size_type _capacity = 0;
        size_type _size = 0;
    };

    inline namespace literals {
        [[nodiscard]] String operator"" _s(char8 const* literal, u64 size);
    }

    [[nodiscard]] bool operator==(String const& lhs, String const& rhs);
    [[nodiscard]] bool operator!=(String const& lhs, String const& rhs);

    String& operator+=(String& lhs, char8 rhs);
    String& operator+=(String& lhs, char32 rhs);
    String& operator+=(String& lhs, String_View rhs);

    [[nodiscard]] String operator+(String const& lhs, String const& rhs);
    [[nodiscard]] String operator+(String_View lhs, String const& rhs);
    [[nodiscard]] String operator+(String const& lhs, String_View rhs);
    [[nodiscard]] String operator+(char8 const* lhs, String const& rhs);
    [[nodiscard]] String operator+(String const& lhs, char8 const* rhs);

    [[nodiscard]] String to_string(i32 v);
    [[nodiscard]] String to_string(u32 v);
    [[nodiscard]] String to_string(i64 v);
    [[nodiscard]] String to_string(u64 v);
    [[nodiscard]] String to_string(f32 v);
    [[nodiscard]] String to_string(f64 v);
    [[nodiscard]] String to_string(void const* v);

    // TODO: Implement in terms of String_View.
    [[nodiscard]] f32 str_to_f32(String const& string);

    // replace
    // Replaces all occurences of pattern in string with replacement.
    //
    // Parameters:
    //      string - the source string to perform replacement on.
    //     pattern - the pattern to be replaced.
    // replacement - the replacement string.
    //
    // Returns:
    // A new string with all occurences of pattern replaced with replacement.
    //
    [[nodiscard]] String replace(String_View string, String_View pattern, String_View replacement);

    template<>
    struct Default_Hash<String> {
        using transparent = void;

        [[nodiscard]] constexpr u64 operator()(String_View const v) const {
            return anton::hash(v);
        }
    };

    template<>
    struct Equal_Compare<String> {
        using transparent = void;

        [[nodiscard]] constexpr bool operator()(String_View const lhs, String_View const rhs) const {
            return lhs == rhs;
        }
    };
} // namespace anton
