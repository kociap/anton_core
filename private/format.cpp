#include <anton/format.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>

namespace anton {
    struct Format_Field {
        bool dont_print;
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

            // Check if we've encountered an escaped brace `{{`.
            auto backup = i;
            if(++i != end) {
                if(*i == U'{') {
                    string_slices.emplace_back(str_slice_begin, i);
                    format_fields.emplace_back(true);
                    ++i;
                    str_slice_begin = i;
                    continue;
                } else {
                    i = backup;
                    string_slices.emplace_back(str_slice_begin, i);
                }
            } else {
                // Invalid string format - missing matching `}`.
                return false;
            }

            // TODO: Currently only supports `{}` as format.
            while(i != end && *i != U'}') {
                ++i;
            }

            if(i != end && *i == U'}') {
                format_fields.emplace_back(false);
                str_slice_begin = ++i;
            } else {
                //Invalid string format - missing matching `}`;
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
            ANTON_FAIL(false, "invalid format string"_sv);
        }

        // Guard against argument/format-field mistmach
        if(format_fields.size() != arguments.size()) {
            ANTON_FAIL(false, "mismatched argument and format fields counts"_sv);
        }

        auto field = format_fields.begin();
        auto field_end = format_fields.end();
        auto args = arguments.begin();
        auto args_end = arguments.end();
        for(auto i = string_slices.begin(), end = string_slices.end(); i != end; ++i) {
            buffer.write(*i);
            if(field != field_end && args != args_end) {
                if(!field->dont_print) {
                    auto arg = *args;
                    arg->format(buffer);
                    ++args;
                }
                ++field;
            }
        }
        return buffer.to_string();
    }

    String detail::format_internal(String_View const format_string, Slice<Formatter_Base const* const> arguments) {
        Default_Format_Buffer buffer;
        return format_internal(buffer, format_string, arguments);
    }
} // namespace anton
