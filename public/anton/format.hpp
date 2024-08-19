#pragma once

#include <anton/allocator.hpp>
#include <anton/slice.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>
#include <anton/type_traits/utility.hpp>

namespace anton {
  struct Format_Buffer {
  public:
    Format_Buffer(Memory_Allocator* const allocator);
    void write(String_View string);
    String to_string();

  private:
    String _string;
  };

  namespace detail {
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     bool value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     i8 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     u8 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     i16 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     u16 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     i32 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     u32 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     i64 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     u64 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     f32 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     f64 value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     void const* value);
    void format_type(Memory_Allocator* const allocator, Format_Buffer& buffer,
                     String_View value);
  } // namespace detail

  struct Formatter_Base {
    virtual ~Formatter_Base() = default;
    virtual void format(Memory_Allocator* const allocator,
                        Format_Buffer& buffer) const = 0;
  };

  template<typename T>
  struct Formatter final: Formatter_Base {
  private:
    using value_type = remove_reference<T>;
    value_type const& value;

  public:
    Formatter(value_type const& v): value(v) {}
    virtual ~Formatter() override = default;

    virtual void format(Memory_Allocator* const allocator,
                        Format_Buffer& buffer) const override
    {
      detail::format_type(allocator, buffer, value);
    }
  };

  template<>
  struct Formatter<char8 const*> final: Formatter_Base {
  private:
    String_View _string;

  public:
    Formatter(char8 const* string): _string(string) {}
    virtual ~Formatter() override = default;

    virtual void format(Memory_Allocator* const allocator,
                        Format_Buffer& buffer) const override
    {
      detail::format_type(allocator, buffer, _string);
    }
  };

  namespace detail {
    String format_internal(Memory_Allocator* const allocator,
                           String_View format_string,
                           Slice<Formatter_Base const* const> args);

    template<typename... Args>
    String format(Memory_Allocator* const allocator,
                  String_View const format_string, Args&&... args)
    {
      if constexpr(sizeof...(Args) > 0) {
        Formatter_Base const* const arguments[sizeof...(Args)] = {&args...};
        return format_internal(allocator, format_string, arguments);
      } else {
        return format_internal(allocator, format_string, {});
      }
    }
  } // namespace detail

  template<typename... Args>
  [[nodiscard]] String format(Memory_Allocator* const allocator,
                              String_View const format_string, Args&&... args)
  {
    return detail::format(allocator, format_string,
                          Formatter<decay<Args>>(ANTON_FWD(args))...);
  }

  template<typename... Args>
  [[nodiscard]] String format(String_View const format_string, Args&&... args)
  {
    return detail::format(get_default_allocator(), format_string,
                          Formatter<decay<Args>>(ANTON_FWD(args))...);
  }
} // namespace anton
