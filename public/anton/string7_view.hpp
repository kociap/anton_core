#pragma once

#include <anton/assert.hpp>
#include <anton/detail/string_common.hpp>
#include <anton/hashing/murmurhash2.hpp>
#include <anton/iterators.hpp>
#include <anton/swap.hpp>
#include <anton/types.hpp>

namespace anton {
    struct String7_View {
    public:
        using size_type = i64;
        using difference_type = isize;
        using value_type = char8;
        using iterator = char8*;
        using const_iterator = char8 const*;

    public:
        constexpr String7_View(): _begin(nullptr), _end(nullptr) {}

        constexpr String7_View(String7_View const&) = default;

        constexpr String7_View(value_type const* str): _begin(str), _end(str) {
            if(_begin != nullptr) {
                while(*_end)
                    ++_end;
            }

            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String7_View(value_type const* str, size_type size): _begin(str), _end(str + size) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String7_View(value_type const* first, value_type const* last): _begin(first), _end(last) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String7_View& operator=(String7_View const&) = default;

        [[nodiscard]] constexpr const_iterator begin() const {
            return _begin;
        }

        [[nodiscard]] constexpr const_iterator cbegin() const {
            return _begin;
        }

        [[nodiscard]] constexpr const_iterator end() const {
            return _end;
        }

        [[nodiscard]] constexpr const_iterator cend() const {
            return _end;
        }

        // Size of the string in bytes
        [[nodiscard]] constexpr size_type size() const {
            return _end - _begin;
        }

        [[nodiscard]] constexpr value_type const* data() const {
            return _begin;
        }

        friend constexpr void swap(String7_View& sv1, String7_View& sv2) {
            swap(sv1._begin, sv2._begin);
            swap(sv1._end, sv2._end);
        }

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    inline namespace literals {
        [[nodiscard]] constexpr String7_View operator"" _sv7(char8 const* literal, u64 size) {
            return String7_View{literal, (i64)size};
        }
    } // namespace literals

    // Compares bytes
    [[nodiscard]] constexpr bool operator==(String7_View const& lhs, String7_View const& rhs) {
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

    // Compares bytes
    [[nodiscard]] constexpr bool operator!=(String7_View const& lhs, String7_View const& rhs) {
        return !(lhs == rhs);
    }

    // compare
    // Orders two strings lexicographically by comparing codepoints.
    //
    // Returns:
    //-1 if lhs < rhs, 0 if lhs == rhs and 1 if lhs > rhs.
    //
    [[nodiscard]] i32 compare(String7_View lhs, String7_View rhs);

    constexpr u64 hash(String7_View const view) {
        return murmurhash2_64(view.begin(), view.size());
    }

    // find_substring
    // Finds the first occurence of substr within string.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to look for in string.
    //
    // Returns:
    // The start position of the substring within string or npos if the substring is not present.
    //
    [[nodiscard]] constexpr i64 find_substring(String7_View const string, String7_View const substr) {
        // Bruteforce
        char8 const* const string_data = string.data();
        char8 const* const substr_data = substr.data();
        for(i64 i = 0, end = string.size() - substr.size(); i <= end; ++i) {
            bool equal = true;
            for(i64 j = i, k = 0; k < substr.size(); ++j, ++k) {
                equal &= string_data[j] == substr_data[k];
            }

            if(equal) {
                return i;
            }
        }
        return npos;
    }

    // find_last_substring
    // Finds the last occurence of substr within string.
    //
    // Parameters:
    // string - the source string.
    // substr - the substring to look for in string.
    //
    // Returns:
    // The start position of the substring within string or npos if the substring is not present.
    //
    [[nodiscard]] constexpr i64 find_last_substring(String7_View const string, String7_View const substr) {
        // Bruteforce
        char8 const* const string_data = string.data();
        char8 const* const substr_data = substr.data();
        for(i64 i = string.size() - substr.size(); i >= 0; --i) {
            bool equal = true;
            for(i64 j = i, k = 0; k < substr.size(); ++j, ++k) {
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
    [[nodiscard]] constexpr bool begins_with(String7_View const string, String7_View const substr) {
        char8 const* str_begin = string.begin();
        char8 const* str_end = string.end();
        char8 const* substr_begin = substr.begin();
        char8 const* substr_end = substr.end();
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
    [[nodiscard]] constexpr bool ends_with(String7_View const string, String7_View const substr) {
        char8 const* str_begin = string.begin() - 1;
        char8 const* str_end = string.end() - 1;
        char8 const* substr_begin = substr.begin() - 1;
        char8 const* substr_end = substr.end() - 1;
        for(; str_end != str_begin && substr_end != substr_begin; --str_begin, --substr_begin) {
            if(*str_end != *substr_end) {
                return false;
            }
        }
        return substr_begin == substr_end;
    }

    // str_to_i64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Accepts both minus and plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] i64 str_to_i64(String7_View string, u64 base = 10);

    // str_to_u64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Does not accepts either minus or plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] u64 str_to_u64(String7_View string, u64 base = 10);
} // namespace anton

namespace std {
    template<typename T>
    struct hash;

    template<>
    struct hash<anton::String7_View> {
        anton::u64 operator()(anton::String7_View const view) const {
            return anton::hash(view);
        }
    };
} // namespace std
