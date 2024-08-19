#include <anton/string7_view.hpp>

namespace anton {
  i32 compare(String7_View const lhs, String7_View const rhs)
  {
    auto lhs_i = lhs.begin(), rhs_i = rhs.begin(), lhs_end = lhs.end(),
         rhs_end = rhs.end();
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

  // str_to_integer
  // - expects a string containing a number in base [2, 36].
  // - there must be no whitespace in the string.
  // - accepts both minus and plus signs only if T is signed. Otherwise accepts
  //   neither.
  // - does not recognize 0b and 0x prefixes.
  // - does not guard against overflow.
  // - if the string is not valid, the return value is unspecified.
  //
  template<typename T>
  [[nodiscard]] T str_to_integer(String7_View const string, u64 const base = 10)
  {
    // Convert ['0', '9'] ['A', 'Z'] ['a', 'z'] to [0, 35], everything else to
    // 255.
    static constexpr u8 byte_to_digit[] = {
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   255, 255,
      255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
      35,  255, 255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,
      33,  34,  35,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255};

    char8 const* first = string.begin();
    bool negative = false;
    if constexpr(is_signed<T>) {
      if(*first == '-' || *first == '+') {
        negative = (*first == '-');
        ++first;
      }
    }

    T number = 0;
    char8 const* last = string.end();
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

  i64 str_to_i64(String7_View const string, u64 const base)
  {
    return str_to_integer<i64>(string, base);
  }

  u64 str_to_u64(String7_View const string, u64 const base)
  {
    return str_to_integer<u64>(string, base);
  }
} // namespace anton
