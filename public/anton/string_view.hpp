#pragma once

#include <anton/assert.hpp>
#include <anton/detail/string8_common.hpp>
#include <anton/detail/string_common.hpp>
#include <anton/functors.hpp>
#include <anton/hashing/murmurhash2.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/swap.hpp>
#include <anton/types.hpp>

namespace anton {
    struct String_View {
    public:
        using size_type = i64;
        using difference_type = isize;
        using value_type = char8;
        using byte_iterator = char8*;
        using byte_const_iterator = char8 const*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        constexpr String_View(): _begin(nullptr), _end(nullptr) {}

        constexpr String_View(String_View const&) = default;

        constexpr String_View(value_type const* str): _begin(str), _end(str) {
            if(_begin != nullptr) {
                while(*_end)
                    ++_end;
            }

            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String_View(value_type const* str, size_type size): _begin(str), _end(str + size) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String_View(value_type const* first, value_type const* last): _begin(first), _end(last) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        String_View(char_iterator first, char_iterator last): _begin(first.get_underlying_pointer()), _end(last.get_underlying_pointer()) {}

        constexpr String_View& operator=(String_View const&) = default;

        // TODO: proxies are non-constexpr
        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes bytes() const {
            return {_begin, _end};
        }

        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes const_bytes() const {
            return {_begin, _end};
        }

        [[nodiscard]] /* constexpr */ UTF8_Chars chars() const {
            return {_begin, _end};
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_begin() const {
            return _begin;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_cbegin() const {
            return _begin;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_end() const {
            return _end;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_cend() const {
            return _end;
        }

        // TODO: iterators are non-constexpr
        [[nodiscard]] /* constexpr */ char_iterator chars_begin() const {
            return char_iterator{_begin, 0};
        }

        [[nodiscard]] /* constexpr */ char_iterator chars_end() const {
            return char_iterator{_end, _end - _begin};
        }

        // Size of the string in bytes
        [[nodiscard]] constexpr size_type size_bytes() const {
            return _end - _begin;
        }

        [[nodiscard]] constexpr value_type const* data() const {
            return _begin;
        }

        friend constexpr void swap(String_View& sv1, String_View& sv2) {
            swap(sv1._begin, sv2._begin);
            swap(sv1._end, sv2._end);
        }

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    inline namespace literals {
        [[nodiscard]] constexpr String_View operator"" _sv(char8 const* literal, u64 size) {
            return String_View{literal, (i64)size};
        }
    } // namespace literals

    // Compares bytes
    [[nodiscard]] constexpr bool operator==(String_View const& lhs, String_View const& rhs) {
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

    // Compares bytes
    [[nodiscard]] constexpr bool operator!=(String_View const& lhs, String_View const& rhs) {
        return !(lhs == rhs);
    }

    // compare
    // Orders two strings lexicographically by comparing codepoints.
    //
    // Returns:
    //-1 if lhs < rhs, 0 if lhs == rhs and 1 if lhs > rhs.
    //
    [[nodiscard]] i32 compare(String_View lhs, String_View rhs);

    [[nodiscard]] constexpr u64 hash(String_View const view) {
        return murmurhash2_64(view.bytes_begin(), view.size_bytes());
    }

    // find_substring
    // Finds the first occurrence of substr within string.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to look for in string.
    //
    // Returns:
    // The start position of the substring within string or npos if the substring is not present.
    //
    [[nodiscard]] constexpr i64 find_substring(String_View const string, String_View const substr) {
        // Bruteforce
        char8 const* const string_data = string.data();
        char8 const* const substr_data = substr.data();
        for(i64 i = 0, end = string.size_bytes() - substr.size_bytes(); i <= end; ++i) {
            bool equal = true;
            for(i64 j = i, k = 0; k < substr.size_bytes(); ++j, ++k) {
                equal &= string_data[j] == substr_data[k];
            }

            if(equal) {
                return i;
            }
        }
        return npos;
    }

    // find_last_substring
    // Finds the last occurrence of substr within string.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to look for in string.
    //
    // Returns:
    // The start position of the substring within string or npos if the substring is not present.
    //
    [[nodiscard]] constexpr i64 find_last_substring(String_View const string, String_View const substr) {
        // Bruteforce
        char8 const* const string_data = string.data();
        char8 const* const substr_data = substr.data();
        for(i64 i = string.size_bytes() - substr.size_bytes(); i >= 0; --i) {
            bool equal = true;
            for(i64 j = i, k = 0; k < substr.size_bytes(); ++j, ++k) {
                equal &= string_data[j] == substr_data[k];
            }

            if(equal) {
                return i;
            }
        }
        return npos;
    }

    // begins_with
    // Checks whether string starts with substr.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to check for.
    //
    // Returns:
    // If string begins with substr, returns true, false otherwise.
    //
    [[nodiscard]] constexpr bool begins_with(String_View const string, String_View const substr) {
        char8 const* str_begin = string.bytes_begin();
        char8 const* str_end = string.bytes_end();
        char8 const* substr_begin = substr.bytes_begin();
        char8 const* substr_end = substr.bytes_end();
        for(; str_begin != str_end && substr_begin != substr_end; ++str_begin, ++substr_begin) {
            if(*str_begin != *substr_begin) {
                return false;
            }
        }
        return substr_begin == substr_end;
    }

    // ends_with
    // Checks whether string ends with substr.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to check for.
    //
    // Returns:
    // If string ends with substr, returns true, false otherwise.
    //
    [[nodiscard]] constexpr bool ends_with(String_View const string, String_View const substr) {
        char8 const* str_begin = string.bytes_begin() - 1;
        char8 const* str_end = string.bytes_end() - 1;
        char8 const* substr_begin = substr.bytes_begin() - 1;
        char8 const* substr_end = substr.bytes_end() - 1;
        for(; str_end != str_begin && substr_end != substr_begin; --str_begin, --substr_begin) {
            if(*str_end != *substr_end) {
                return false;
            }
        }
        return substr_begin == substr_end;
    }

    // TODO: Implement shrink_chars functions.

    // shrink_front_bytes
    // Shrink the string from the front by count bytes.
    // Does not validate that the obtained substring is a valid unicode string.
    //
    // Parameters:
    // string - the source string.
    //  count - the number of bytes to shave off the front of the string. Must be positive.
    //
    // Returns:
    // String shrunk at the front by count bytes.
    //
    [[nodiscard]] constexpr String_View shrink_front_bytes(String_View const string, i64 const count) {
        ANTON_ASSERT(count >= 0, "shrink count must be positive");
        i64 const shrink_size = math::min(string.size_bytes(), count);
        return String_View{string.bytes_begin() + shrink_size, string.bytes_end()};
    }

    // shrink_back_bytes
    // Shrink the string from the back by count bytes.
    // Does not validate that the obtained substring is a valid unicode string.
    //
    // Parameters:
    // string - the source string.
    //  count - the number of bytes to shave off the back of the string. Must be positive.
    //
    // Returns:
    // String shrunk at the back by count bytes.
    //
    [[nodiscard]] constexpr String_View shrink_back_bytes(String_View const string, i64 const count) {
        ANTON_ASSERT(count >= 0, "shrink count must be positive");
        i64 const shrink_size = math::min(string.size_bytes(), count);
        return String_View{string.bytes_begin(), string.bytes_end() - shrink_size};
    }

    // shrink_bytes
    // Shrink the string from both ends by front_count and back_count bytes respectively.
    // Does not validate that the obtained substring is a valid unicode string.
    //
    // Parameters:
    //      string - the source string.
    // front_count - the number of bytes to shave off the front of the string. Must be positive.
    //  back_count - the number of bytes to shave off the back of the string. Must be positive.
    //
    // Returns:
    // String shrunk on both ends by front_count and back_count bytes respectively.
    //
    [[nodiscard]] constexpr String_View shrink_bytes(String_View const string, i64 const front_count, i64 const back_count) {
        ANTON_ASSERT(front_count >= 0, "shrink front_count must be positive");
        ANTON_ASSERT(back_count >= 0, "shrink back_count must be positive");
        return shrink_back_bytes(shrink_front_bytes(string, front_count), back_count);
    }

    // str_to_i64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Accepts both minus and plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] i64 str_to_i64(String_View string, u64 base = 10);

    // str_to_u64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Does not accepts either minus or plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] u64 str_to_u64(String_View string, u64 base = 10);

    template<>
    struct Default_Hash<String_View> {
        using transparent = void;

        [[nodiscard]] constexpr u64 operator()(String_View const v) const {
            return anton::hash(v);
        }
    };

    template<>
    struct Equal_Compare<String_View> {
        using transparent = void;

        [[nodiscard]] constexpr bool operator()(String_View const lhs, String_View const rhs) const {
            return lhs == rhs;
        }
    };
} // namespace anton

namespace std {
    template<typename T>
    struct hash;

    template<>
    struct hash<anton::String_View> {
        anton::u64 operator()(anton::String_View const view) const {
            return anton::hash(view);
        }
    };
} // namespace std
