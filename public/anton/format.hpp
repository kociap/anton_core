#pragma once

#include <anton/string.hpp>
#include <anton/string_view.hpp>

namespace anton {
    struct Formatter {
        virtual ~Formatter() = default;
        virtual void write(String_View string) = 0;
    };

    void format_type(Formatter& formatter, i8 value);
    void format_type(Formatter& formatter, u8 value);
    void format_type(Formatter& formatter, i16 value);
    void format_type(Formatter& formatter, u16 value);
    void format_type(Formatter& formatter, i32 value);
    void format_type(Formatter& formatter, u32 value);
    void format_type(Formatter& formatter, i64 value);
    void format_type(Formatter& formatter, u64 value);
    void format_type(Formatter& formatter, f32 value);
    void format_type(Formatter& formatter, f64 value);
    void format_type(Formatter& formatter, void const* value);
    void format_type(Formatter& formatter, String_View value);

    namespace detail {
        struct Format_Argument_Base {
            void const* value;

            Format_Argument_Base(void const* p): value(p) {}
            virtual ~Format_Argument_Base() = default;
            virtual void format(Formatter& formatter) const = 0;
        };

        template<typename T>
        struct Format_Argument final: Format_Argument_Base {
            Format_Argument(T const& p): Format_Argument_Base(&p) {}
            virtual ~Format_Argument() override = default;

            virtual void format(Formatter& formatter) const override {
                if constexpr(is_reference<T>) {
                    format_type(formatter, *(remove_reference<T> const*)value);
                } else {
                    format_type(formatter, *(T const*)value);
                }
            }
        };

        template<usize N>
        struct Format_Argument<char8 const (&)[N]> final: Format_Argument_Base {
            Format_Argument(char8 const (&array)[N]): Format_Argument_Base(array) {}
            virtual ~Format_Argument() override = default;

            virtual void format(Formatter& formatter) const override {
                char8 const* str = (char8 const*)value;
                format_type(formatter, String_View{str, str + N - 1});
            }
        };

        String format_internal(String_View const format_string, Format_Argument_Base const* const* args, i64 args_size);

        template<typename... Args>
        String format(String_View const format_string, Args const&... args) {
            Format_Argument_Base const* const arguments[sizeof...(Args)] = {&args...};
            return format_internal(format_string, arguments, sizeof...(Args));
        }
    } // namespace detail

    template<typename... Args>
    String format(String_View const format_string, Args&&... args) {
        return detail::format(format_string, detail::Format_Argument<Args>(args)...);
    }
} // namespace anton
