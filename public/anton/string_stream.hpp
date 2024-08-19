#pragma once

#include <anton/stream.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>

namespace anton {
  struct Input_String_Stream: public Input_Stream {
  public:
    Input_String_Stream(String_View string);
    Input_String_Stream(String string);

    [[nodiscard]] virtual operator bool() const override;

    // read
    //
    // Returns:
    // The number of bytes read.
    //
    virtual i64 read(void* buffer, i64 count) override;

    // read
    //
    // Returns:
    // The number of bytes read.
    //
    virtual i64 read(Slice<u8> buffer) override;

    [[nodiscard]] virtual char32 peek() override;
    virtual char32 get() override;
    virtual void unget() override;
    // The only valid value for offset is a value previously returned by tell.
    // The only allowed value for dir is Seek_Dir::beg. If dir is not
    // Seek_Dir::beg, this function does nothing.
    virtual void seek(Seek_Dir dir, i64 offset) override;
    [[nodiscard]] virtual i64 tell() override;

  private:
    String _string;
    UTF8_Char_Iterator _current;
  };
} // namespace anton
