#include <anton/format.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>

namespace anton {
    static bool parse_format_string(String_View const string, Array<String_View>& string_slices, Array<String_View>& format_fields) {
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

            auto format_field_begin = i;
            if(*i == U'{') {
                // We have found an escaped brace.
                continue;
            } else {
                // We have found a format field.
                string_slices.emplace_back(str_slice_begin, backup);
                i = backup;
            }

            while(i != end && *i != U'}') {
                ++i;
            }

            if(i == end) {
                // Invalid string format - missing matching `}`.
                return false;
            }

            format_fields.emplace_back(format_field_begin, i);
            str_slice_begin = ++i;
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
        Array<String_View> format_fields;
        if(!parse_format_string(format_string, string_slices, format_fields)) {
            ANTON_FAIL(false, "invalid format string");
        }

        // Guard against argument/format-field mistmach
        if(format_fields.size() != arguments.size()) {
            ANTON_FAIL(false, "mismatched argument and format fields counts");
        }

        // We don't use format_field yet. We should pass it to the argument format function.
        // auto format_field = format_fields.begin();
        auto argument_first = arguments.begin();
        auto argument_last = arguments.end();
        for(auto i = string_slices.begin(), end = string_slices.end(); i != end; ++i) {
            buffer.write(*i);
            // We don't have to check format_field for end
            // because we have already ensured those ranges are equal.
            if(argument_first != argument_last) {
                Formatter_Base const* const argument = *argument_first;
                argument->format(buffer);
                ++argument_first;
                // ++format_field;
            }
        }
        return buffer.to_string();
    }

    String detail::format_internal(String_View const format_string, Slice<Formatter_Base const* const> arguments) {
        Default_Format_Buffer buffer;
        return format_internal(buffer, format_string, arguments);
    }
} // namespace anton
