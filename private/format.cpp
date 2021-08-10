#include <anton/format.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>
#include <anton/optional.hpp>

namespace anton {
    struct Format_Field {
        String_View format;
        bool print;
    };

    static bool parse_format_string(String_View const string, Array<String_View>& string_slices, Array<Format_Field>& format_fields) {
        auto i = string.chars_begin();
        auto const end = string.chars_end();
        auto str_slice_begin = i;
        while(true) {
            while(i != end && *i != U'{') {
                ++i;
            }

            if(i == end) {
                string_slices.emplace_back(str_slice_begin, i);
                return true;
            }

            // Check whether we've encountered an escaped brace `{{`.
            auto backup = i;
            ++i;
            if(i == end) {
                // Invalid string format - missing matching `}`.
                return false;
            }

            auto format_begin = i;
            if(*i == U'{') {
                string_slices.emplace_back(str_slice_begin, i);
                format_fields.emplace_back(String_View{}, false);
                str_slice_begin = ++i;
                continue;
            } else {
                string_slices.emplace_back(str_slice_begin, backup);
            }

            while(i != end && *i != U'}') {
                ++i;
            }

            if(i != end && *i == U'}') {
                format_fields.emplace_back(String_View{format_begin, i}, true);
                str_slice_begin = ++i;
            } else {
                // Invalid string format - missing matching `}`.
                return false;
            }
        }
    }

    struct Default_Format_Buffer final: public Format_Buffer {
    private:
        String _string;

    public:
        virtual ~Default_Format_Buffer() override = default;

        virtual void write(String_View string) override {
            _string += string;
        }

        virtual String to_string() override {
            return ANTON_MOV(_string);
        }
    };

    void format_type(Format_Buffer& format_buffer, i8 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, u8 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, i16 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, u16 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, i32 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, u32 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, i64 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, u64 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, f32 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, f64 value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, void const* value) {
        String str = to_string(value);
        format_buffer.write(str);
    }

    void format_type(Format_Buffer& format_buffer, String_View value) {
        format_buffer.write(value);
    }

    String detail::format_internal(Format_Buffer& buffer, String_View const format_string, Slice<Formatter_Base const* const> const arguments) {
        Array<String_View> string_slices;
        Array<Format_Field> format_fields;
        if(!parse_format_string(format_string, string_slices, format_fields)) {
            ANTON_FAIL(false, "invalid format string");
        }

        auto field = format_fields.begin();
        auto const field_end = format_fields.end();
        auto args = arguments.begin();
        auto const args_end = arguments.end();
        for(String_View const slice: string_slices) {
            buffer.write(slice);
            if(field != field_end) {
                if(!field->print) {
                    ++field;
                } else if(args != args_end) {
                    auto arg = *args;
                    arg->format(buffer);
                    ++args;
                    ++field;
                }
            }
        }

        if(field != field_end || args != args_end) {
            ANTON_FAIL(false, "incorrect number of arguments");
        }

        return buffer.to_string();
    }

    String detail::format_internal(String_View const format_string, Slice<Formatter_Base const* const> arguments) {
        Default_Format_Buffer buffer;
        return format_internal(buffer, format_string, arguments);
    }
} // namespace anton
