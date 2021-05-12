#include <anton/string_view.hpp>

namespace anton {
    i32 compare(String_View const lhs, String_View const rhs) {
        auto lhs_i = lhs.chars_begin(), rhs_i = rhs.chars_begin(), lhs_end = lhs.chars_end(), rhs_end = rhs.chars_end();
        for(; lhs_i != lhs_end && rhs_i != rhs_end; ++lhs_i, ++rhs_i) {
            char32 const c1 = *lhs_i;
            char32 const c2 = *rhs_i;
            if(c1 < c2) {
                return -1;
            } else if(c1 > c2) {
                return 1;
            }
        }

        if(lhs_i == lhs_end && rhs_i != rhs_end) {
            return -1;
        } else if(lhs_i != lhs_end && rhs_i == rhs_end) {
            return 1;
        } else {
            return 0;
        }
    }

    i64 find_last_substring(String_View const string, String_View const substr) {
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

    // str_to_integer
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Accepts both minus and plus signs only if T is signed. Otherwise accepts neither.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    template<typename T>
    [[nodiscard]] T str_to_integer(String_View const string, u64 const base = 10) {
        // Convert ['0', '9'] ['A', 'Z'] ['a', 'z'] to [0, 35], everything else to 255.
        static constexpr u8 byte_to_digit[] = {
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
            255, 255, 255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
            32,  33,  34,  35,  255, 255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
            29,  30,  31,  32,  33,  34,  35,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

        char8 const* first = string.bytes_begin();
        bool negative = false;
        if constexpr(is_signed<T>) {
            if(*first == '-' || *first == '+') {
                negative = (*first == '-');
                ++first;
            }
        }

        T number = 0;
        char8 const* last = string.bytes_end();
        for(; first != last; ++first) {
            u8 const mapped = byte_to_digit[static_cast<u8>(*first)];
            if(mapped <= base) {
                number = number * base + mapped;
            } else {
                break;
            }
        }

        if constexpr(is_signed<T>) {
            if(negative) {
                number = -number;
            }
        }

        return number;
    }

    i64 str_to_i64(String_View const string, u64 const base) {
        return str_to_integer<i64>(string, base);
    }

    u64 str_to_u64(String_View const string, u64 const base) {
        return str_to_integer<u64>(string, base);
    }
} // namespace anton
