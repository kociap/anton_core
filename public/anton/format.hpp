#pragma once

#include <anton/slice.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>
#include <anton/type_traits/utility.hpp>

namespace anton {
    struct Format_Buffer {
        virtual ~Format_Buffer() = default;
        virtual void write(String_View string) = 0;
        virtual String to_string() = 0;
    };

    void format_type(Format_Buffer& buffer, i8 value);
    void format_type(Format_Buffer& buffer, u8 value);
    void format_type(Format_Buffer& buffer, i16 value);
    void format_type(Format_Buffer& buffer, u16 value);
    void format_type(Format_Buffer& buffer, i32 value);
    void format_type(Format_Buffer& buffer, u32 value);
    void format_type(Format_Buffer& buffer, i64 value);
    void format_type(Format_Buffer& buffer, u64 value);
    void format_type(Format_Buffer& buffer, f32 value);
    void format_type(Format_Buffer& buffer, f64 value);
    void format_type(Format_Buffer& buffer, void const* value);
    void format_type(Format_Buffer& buffer, String_View value);

    struct Formatter_Base {
        virtual ~Formatter_Base() = default;
        virtual void format(Format_Buffer& buffer) const = 0;
    };

    template<typename T>
    struct Formatter final: Formatter_Base {
    public:
        using value_type = remove_reference<T>;

    private:
        value_type const& value;

    public:
        Formatter(value_type const& v): value(v) {}
        virtual ~Formatter() override = default;

        virtual void format(Format_Buffer& buffer) const override {
            format_type(buffer, value);
        }
    };

    template<usize N>
    struct Formatter<char8 const (&)[N]> final: Formatter_Base {
    public:
        using value_type = String_View;

    private:
        String_View string;

    public:
        Formatter(char8 const (&array)[N]): string(array, array + N - 1) {} // -1 because of null-terminator
        virtual ~Formatter() override = default;

        virtual void format(Format_Buffer& buffer) const override {
            format_type(buffer, string);
        }
    };

    namespace detail {
        String format_internal(Format_Buffer& buffer, String_View format_string, Slice<Formatter_Base const* const> arguments);
        String format_internal(String_View format_string, Slice<Formatter_Base const* const> args);

        template<typename... Args>
        String format(Format_Buffer& buffer, String_View const format_string, Args&&... args) {
            Formatter_Base const* const arguments[sizeof...(Args)] = {&args...};
            return format_internal(buffer, format_string, arguments);
        }

        template<typename... Args>
        String format(String_View const format_string, Args&&... args) {
            Formatter_Base const* const arguments[sizeof...(Args)] = {&args...};
            return format_internal(format_string, arguments);
        }
    } // namespace detail

    template<typename... Args>
    String format(Format_Buffer& buffer, String_View const format_string, Args&&... args) {
        return detail::format(buffer, format_string, Formatter<Args>(ANTON_FWD(args))...);
    }

    template<typename... Args>
    String format(String_View const format_string, Args&&... args) {
        return detail::format(format_string, Formatter<Args>(ANTON_FWD(args))...);
    }
} // namespace anton
