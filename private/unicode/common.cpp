#include <anton/unicode/common.hpp>

#include <anton/math/math.hpp>

namespace anton::unicode {
    i64 get_byte_count_from_utf8_leading_byte(char8 leading_byte) {
        u8 const leading_zeros = math::clz((u8)~leading_byte);
        i64 const byte_count = math::max((u8)1, leading_zeros);
        return byte_count;
    }

    static i64 get_utf8_bytes_count_in_utf32_codepoint(char32 const codepoint) {
        u32 const leading_zeros = math::clz(codepoint);
        u32 const highest_bit = 32 - leading_zeros;
        // The max number of bits in x bytes is:
        //  - 7 bits in 1 byte
        //  - 11 bits in 2 bytes
        //  - 16 bits in 3 bytes
        //  - 21 bits in 4 bytes
        // Since the max bit count increases by 5 per added byte, we can calculate
        // the number of bytes by performing integer division by 5.
        u32 const byte_count = (highest_bit - 7 + 4) / 5 + 1;
        return byte_count;
    }

    static char32 surrogate_pair_to_codepoint(char16 const high_surrogate, char16 const low_surrogate) {
        // Subtract 0xD800 from high surrogate. Subtract 0xDC00 from low surrogate.
        // Subtract 0x10000 from the merged surrogates.
        // 0x35FDC00 = (0xD800 << 10) + 0xDC00 - 0x10000
        char32 const codepoint = (((high_surrogate & 0x3FF) << 10) | (low_surrogate & 0x3FF)) - 0x35FDC00;
        return codepoint;
    }

    struct Codepoint_Conversion_Result {
        char32 codepoint;
        i32 bytes_read;
    };

    static Codepoint_Conversion_Result utf8_bytes_to_codepoint(char8* bytes) {
        char8 const leading_byte = *bytes;
        // Copied from get_byte_count_from_utf8_leading_byte because we need the leading zeroes.
        u8 const leading_zeros = math::clz((u8)~leading_byte);
        i64 const byte_count = math::max((u8)1, leading_zeros);
        char32 codepoint = leading_byte & (0xFF >> (leading_zeros + 1));
        for(u32 i = 1; i < byte_count; ++i) {
            codepoint <<= 6;
            codepoint |= bytes[i] & 0x3F;
        }
        return {codepoint, byte_count};
    }

    i64 convert_utf32_to_utf8(char32 const* buffer_utf32, i64 const count, char8* buffer_utf8) {
        if constexpr(ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if(buffer_utf8 == nullptr) {
            i64 bytes_read = 0;
            i64 bytes = 0;
            while(true) {
                char32 const codepoint = *buffer_utf32;
                i64 const byte_count = get_utf8_bytes_count_in_utf32_codepoint(codepoint);
                bytes_read += 4;
                bytes += byte_count;
                buffer_utf32 += 1;
                if((count == -1 && codepoint == U'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes;
                }
            }
        } else {
            i64 bytes_read = 0;
            i64 bytes_written = 0;
            while(true) {
                char32 const codepoint = *buffer_utf32;
                i64 const byte_count = get_utf8_bytes_count_in_utf32_codepoint(codepoint);
                for(i64 i = 0; i < byte_count; ++i) {
                    char8 const mask = (0x7F & (!!i - 1)) | (0x3F & (!i - 1));
                    char8 const byte = (codepoint >> (6 * i)) & mask;
                    buffer_utf8[i] = byte;
                }
                bytes_read += 4;
                bytes_written += byte_count;
                buffer_utf32 += 1;
                buffer_utf8 += byte_count;
                if((count == -1 && codepoint == U'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes_written;
                }
            }
        }
    }

    i64 convert_codepoint_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8) {
        if constexpr(ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        // U+0000 to U+D7FF and U+E000 to U+FFFF are encoded as single 16bit chars, which we expect to be the more common case.
        char16 const high_surrogate = *buffer_utf16;
        if(high_surrogate <= 0xD7FF || high_surrogate >= 0xE000) {
            char32 const codepoint = high_surrogate;
            if(codepoint <= 0x7F) {
                *buffer_utf8 = static_cast<char8>(codepoint);
                return 1;
            } else if(codepoint <= 0x7FF) {
                buffer_utf8[0] = 0xC0 | ((codepoint & 0x7C0) >> 6);
                buffer_utf8[1] = 0x80 | (codepoint & 0x3F);
                return 2;
            } else {
                buffer_utf8[0] = 0xE0 | ((codepoint & 0xF000) >> 12);
                buffer_utf8[1] = 0x80 | ((codepoint & 0xFC0) >> 6);
                buffer_utf8[2] = 0x80 | (codepoint & 0x3F);
                return 3;
            }
        } else {
            char16 const low_surrogate = buffer_utf16[1];
            char32 const codepoint = surrogate_pair_to_codepoint(high_surrogate, low_surrogate);
            buffer_utf8[0] = 0xF0 | ((codepoint & 0x1C0000) >> 18);
            buffer_utf8[1] = 0x80 | ((codepoint & 0x3F000) >> 12);
            buffer_utf8[2] = 0x80 | ((codepoint & 0xFC0) >> 6);
            buffer_utf8[3] = 0x80 | (codepoint & 0x3F);
            return 4;
        }
    }

    i64 convert_utf16_to_utf8(char16 const* buffer_utf16, i64 const count, char8* buffer_utf8) {
        if constexpr(ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if(buffer_utf8 == nullptr) {
            i64 bytes = 0;
            i64 bytes_read = 0;
            while(true) {
                // U+0000 to U+D7FF and U+E000 to U+FFFF are encoded as single 16bit chars, which we expect to be the more common case.
                char16 const high_surrogate = *buffer_utf16;
                if(high_surrogate <= 0xD7FF || high_surrogate >= 0xE000) {
                    char16 const codepoint = high_surrogate;
                    // If codepoint is less than 0x7FF, it's 2 bytes in UTF-8. If it's less than 0x7F, it's 1 byte in UTF-8.
                    // Otherwise it's 3 bytes.
                    bytes += 3 - (codepoint <= 0x7FF) - (codepoint <= 0x7F);
                    bytes_read += 2;
                    buffer_utf16 += 1;
                    if((count == -1 && codepoint == u'\0') || (count != -1 && bytes_read >= count)) {
                        return bytes;
                    }
                } else {
                    bytes += 4;
                    bytes_read += 4;
                    buffer_utf16 += 2;
                    if(count != -1 && bytes_read >= count) {
                        return bytes;
                    }
                }
            }
        } else {
            i64 bytes_written = 0;
            i64 bytes_read = 0;
            while(true) {
                // U+0000 to U+D7FF and U+E000 to U+FFFF are encoded as single 16bit chars, which we expect to be the more common case.
                char16 const high_surrogate = *buffer_utf16;
                if(high_surrogate <= 0xD7FF || high_surrogate >= 0xE000) {
                    char32 const codepoint = high_surrogate;
                    if(codepoint <= 0x7F) {
                        *buffer_utf8 = static_cast<char8>(codepoint);
                        buffer_utf8 += 1;
                        bytes_written += 1;
                    } else if(codepoint <= 0x7FF) {
                        buffer_utf8[0] = 0xC0 | ((codepoint & 0x7C0) >> 6);
                        buffer_utf8[1] = 0x80 | (codepoint & 0x3F);
                        buffer_utf8 += 2;
                        bytes_written += 2;
                    } else {
                        buffer_utf8[0] = 0xE0 | ((codepoint & 0xF000) >> 12);
                        buffer_utf8[1] = 0x80 | ((codepoint & 0xFC0) >> 6);
                        buffer_utf8[2] = 0x80 | (codepoint & 0x3F);
                        buffer_utf8 += 3;
                        bytes_written += 3;
                    }
                    bytes_read += 2;
                    buffer_utf16 += 1;
                    if((count == -1 && codepoint == u'\0') || (count != -1 && bytes_read >= count)) {
                        return bytes_written;
                    }
                } else {
                    char16 const low_surrogate = buffer_utf16[1];
                    char32 const codepoint = surrogate_pair_to_codepoint(high_surrogate, low_surrogate);
                    buffer_utf8[0] = 0xF0 | ((codepoint & 0x1C0000) >> 18);
                    buffer_utf8[1] = 0x80 | ((codepoint & 0x3F000) >> 12);
                    buffer_utf8[2] = 0x80 | ((codepoint & 0xFC0) >> 6);
                    buffer_utf8[3] = 0x80 | (codepoint & 0x3F);
                    buffer_utf8 += 4;
                    bytes_written += 4;
                    bytes_read += 4;
                    buffer_utf16 += 2;
                    if(count != -1 && bytes_read >= count) {
                        return bytes_written;
                    }
                }
            }
        }
    }

    i64 convert_utf8_to_utf16(char8 const* buffer_utf8, i64 count, char16* buffer_utf16) {
        if constexpr(ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        i64 bytes = 0;
        i64 bytes_read = 0;
        if(buffer_utf16 == nullptr) {
            while(true) {
                auto const [codepoint, byte_count] = utf8_bytes_to_codepoint(buffer_utf8);
                buffer_utf8 += byte_count;
                bytes_read += byte_count;
                // U+0000 to U+D7FF and U+E000 to U+FFFF are encoded as single 16bit chars.
                bool const is_surrogate_pair = !(codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF));
                bytes += 2 * (is_surrogate_pair + 1);
                if((count == -1 && codepoint == U'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes;
                }
            }
        } else {
            while(true) {
                auto const [codepoint, byte_count] = utf8_bytes_to_codepoint(buffer_utf8);
                buffer_utf8 += byte_count;
                bytes_read += byte_count;
                // U+0000 to U+D7FF and U+E000 to U+FFFF are encoded as single 16bit chars, which we expect to be the more common case.
                if(codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF)) {
                    *buffer_utf16 = codepoint;
                    buffer_utf16 += 1;
                    bytes += 2;
                } else {
                    char16 const high_surrogate = 0xD800 + ((codepoint - 0x10000) >> 10 & 0x3FF);
                    char16 const low_surrogate = 0xDC00 + ((codepoint - 0x10000) & 0x3FF);
                    buffer_utf16[0] = high_surrogate;
                    buffer_utf16[1] = low_surrogate;
                    buffer_utf16 += 2;
                    bytes += 4;
                }

                if((count == -1 && codepoint == U'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes;
                }
            }
        }
    }

    i64 convert_utf8_to_utf32(char8 const* buffer_utf8, i64 const count, char32* buffer_utf32) {
        if constexpr(ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        i64 bytes = 0;
        i64 bytes_read = 0;
        if(buffer_utf32 == nullptr) {
            while(true) {
                u8 const leading_byte = *buffer_utf8;
                u32 const byte_count = get_byte_count_from_utf8_leading_byte(leading_byte);
                bytes += 4;
                bytes_read += byte_count;
                buffer_utf8 += byte_count;
                if((count == -1 && leading_byte == u8'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes;
                }
            }
        } else {
            while(true) {
                u8 const leading_byte = *buffer_utf8;
                auto const [codepoint, byte_count] = utf8_bytes_to_codepoint(buffer_utf8);
                *buffer_utf32 = codepoint;
                bytes += 4;
                bytes_read += byte_count;
                buffer_utf8 += byte_count;
                buffer_utf32 += 1;
                if((count == -1 && leading_byte == u8'\0') || (count != -1 && bytes_read >= count)) {
                    return bytes;
                }
            }
        }
    }
} // namespace anton::unicode
