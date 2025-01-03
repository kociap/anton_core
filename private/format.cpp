#include <anton/format.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>
#include <anton/optional.hpp>

namespace anton {
  Format_Buffer::Format_Buffer(Memory_Allocator* const allocator)
    : _string(allocator)
  {
  }

  void Format_Buffer::write(String_View string)
  {
    _string += string;
  }

  String Format_Buffer::to_string()
  {
    return ANTON_MOV(_string);
  }

  namespace detail {
    void format_type([[maybe_unused]] Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, bool value)
    {
      String_View str = (value ? u8"true"_sv : u8"false"_sv);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, i8 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, u8 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, i16 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, u16 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, i32 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, u32 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, i64 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, u64 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, f32 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, f64 value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type(Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, void const* value)
    {
      String str = to_string(allocator, value);
      format_buffer.write(str);
    }

    void format_type([[maybe_unused]] Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, String_View value)
    {
      format_buffer.write(value);
    }

    void format_type([[maybe_unused]] Memory_Allocator* const allocator,
                     Format_Buffer& format_buffer, String7_View value)
    {
      anton::String_View string{value.begin(), value.end()};
      format_buffer.write(string);
    }
  } // namespace detail

  struct Format_Field {
    String_View format;
    bool print;
  };

  static bool parse_format_string(String_View const string,
                                  Array<String_View>& string_slices,
                                  Array<Format_Field>& format_fields)
  {
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

  String
  detail::format_internal(Memory_Allocator* const allocator,
                          String_View const format_string,
                          Slice<Formatter_Base const* const> const arguments)
  {
    Array<String_View> string_slices;
    Array<Format_Field> format_fields;
    if(!parse_format_string(format_string, string_slices, format_fields)) {
      ANTON_FAIL(false, "invalid format string");
    }

    Format_Buffer buffer(allocator);
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
          arg->format(allocator, buffer);
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
} // namespace anton
