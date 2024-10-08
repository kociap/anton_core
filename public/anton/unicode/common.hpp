#pragma once

#include <anton/types.hpp>

namespace anton::unicode {
  // get_byte_count_from_utf8_leading_byte
  // Calculates the number of bytes in a UTF-8 encoded codepoint from the
  // leading byte of the sequence.
  //
  // Returns:
  // The number of bytes in the UTF-8 encoded codepoint.
  //
  i64 get_byte_count_from_utf8_leading_byte(char8 leading_byte);

  // convert_utf32_to_utf8
  // Converts a UTF-32 encoded string contained in buffer_utf32 to a UTF-8
  // encoded string and writes it to buffer_utf8.
  // If buffer_utf8 is nullptr, this function calculates the size in bytes
  // required to fit the converted string in buffer_utf8.
  // count is the number of bytes in buffer_utf32 to be converted. If count is
  // -1, the function will convert all charcaters up until and including
  // null-terminator.
  //
  // Returns:
  // The number of UTF-8 bytes written or the size of buffer_utf8 in bytes
  // required to store the converted string if buffer_utf8 is nullptr.
  //
  // Exceptions:
  // If buffer_utf32 does not contain valid UTF-32, then the function raises
  // assertion if ANTON_UNICODE_VALIDATE_ENCODING is defined.
  // Otherwise the behaviour is undefined.
  //
  i64 convert_utf32_to_utf8(char32 const* buffer_utf32, i64 count,
                            char8* buffer_utf8);

  // convert_codepoint_utf16_to_utf8
  // Converts a single UTF-16 codepoint contained in buffer_utf16 to UTF-8
  // codepoint and writes it to buffer_utf8.
  // If the UTF-16 encoded codepoint is only 2 bytes, this function does not
  // access the second char16.
  //
  // Returns:
  // The number of UTF-8 bytes written.
  //
  // Exceptions:
  // If buffer_utf16 does not contain valid UTF-16, then the function raises
  // assertion if ANTON_UNICODE_VALIDATE_ENCODING is defined.
  // Otherwise the behaviour is undefined.
  //
  i64 convert_codepoint_utf16_to_utf8(char16 const* buffer_utf16,
                                      char8* buffer_utf8);

  // convert_utf16_to_utf8
  // Converts a UTF-16 encoded string contained in buffer_utf16 to a UTF-8
  // encoded string and writes it to buffer_utf8.
  // If buffer_utf8 is nullptr, this function calculates the size in bytes
  // required to fit the converted string in buffer_utf8.
  // count is the number of bytes in buffer_utf16 to be converted. If count is
  // -1, the function will convert all charcaters up until and including
  // null-terminator.
  //
  // Returns:
  // The number of UTF-8 bytes written or the size of buffer_utf8 in bytes
  // required to store the converted string if buffer_utf8 is nullptr.
  //
  // Exceptions:
  // If buffer_utf16 does not contain valid UTF-16, then the function raises
  // assertion if ANTON_UNICODE_VALIDATE_ENCODING is defined.
  // Otherwise the behaviour is undefined.
  //
  i64 convert_utf16_to_utf8(char16 const* buffer_utf16, i64 count,
                            char8* buffer_utf8);

  // convert_utf8_to_utf16
  // Converts a UTF-8 encoded string contained in buffer_utf8 to a UTF-16
  // encoded string and writes it to buffer_utf16.
  // If buffer_utf16 is nullptr, this function calculates the size in bytes
  // required to fit the converted string in buffer_utf16.
  // count is the number of bytes in buffer_utf8 to be converted. If count is
  // -1, the function will convert all charcaters up until and including
  // null-terminator.
  //
  // Returns:
  // The number of UTF-16 bytes written or if buffer_utf16 is nullptr, the size
  // of buffer_utf16 in bytes required to store the converted string.
  //
  // Exceptions:
  // If buffer_utf8 does not contain valid UTF-8, then the function raises
  // assertion if ANTON_UNICODE_VALIDATE_ENCODING is defined.
  // Otherwise the behaviour is undefined.
  //
  i64 convert_utf8_to_utf16(char8 const* buffer_utf8, i64 count,
                            char16* buffer_utf16);

  // convert_codepoint_utf8_to_utf32
  // Converts a single UTF-8 encoded codepoint contained in buffer_utf8 into a
  // UTF-32 encoded codepoint.
  // This function accesses only as many bytes as indicated by the leading byte.
  //
  // Parameters:
  // buffer_utf8 - a buffer containing the UTF-8 encoded codepoint.
  //
  // Returns:
  // UTF-32 encoded codepoint.
  //
  char32 convert_codepoint_utf8_to_utf32(char8 const* buffer_utf8);

  // convert_utf8_to_utf32
  // Converts a UTF-8 encoded string contained in buffer_utf8 to a UTF-32
  // encoded string and writes it to buffer_utf32.
  // If buffer_utf32 is nullptr, this function calculates the size in bytes
  // required to fit the converted string in buffer_utf32.
  // count is the number of bytes in buffer_utf8 to be converted. If count is
  // -1, the function will convert all charcaters up until and including
  // null-terminator.
  //
  // Returns:
  // The number of UTF-32 bytes written or the size of buffer_utf32 in bytes
  // required to store the converted string if buffer_utf32 is nullptr.
  //
  // Exceptions:
  // If buffer_utf8 does not contain valid UTF-8, then the function raises
  // assertion if ANTON_UNICODE_VALIDATE_ENCODING is defined.
  // Otherwise the behaviour is undefined.
  //
  i64 convert_utf8_to_utf32(char8 const* buffer_utf8, i64 count,
                            char32* buffer_utf32);
} // namespace anton::unicode
