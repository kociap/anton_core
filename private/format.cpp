#include <anton/format.hpp>

#include <anton/array.hpp>

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

    struct Default_Formatter final: public Formatter {
        String* str;

        Default_Formatter(String* str): str(str) {}
        virtual ~Default_Formatter() override = default;

        virtual void write(String_View string) override {
            *str += string;
        }
    };

    void format_type(Formatter& formatter, i8 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, u8 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, i16 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, u16 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, i32 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, u32 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, i64 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, u64 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, f32 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, f64 value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, void const* value) {
        String str = to_string(value);
        formatter.write(str);
    }

    void format_type(Formatter& formatter, String_View value) {
        formatter.write(value);
    }

    String detail::format_internal(String_View const format_string, detail::Format_Argument_Base const* const* args, i64 const args_size) {
        Array<String_View> string_slices;
        Array<Format_Field> format_fields;
        if(!parse_format_string(format_string, string_slices, format_fields)) {
            return String{""};
        }

        String out;
        Default_Formatter formatter{&out};
        auto field = format_fields.begin();
        auto field_end = format_fields.end();
        auto args_end = args + args_size;
        for(auto i = string_slices.begin(), end = string_slices.end(); i != end; ++i) {
            formatter.write(*i);
            if(field != field_end && args != args_end) {
                if(!field->dont_print) {
                    auto arg = *args;
                    arg->format(formatter);
                    ++args;
                }
                ++field;
            }
        }
        return out;
    }
} // namespace anton
