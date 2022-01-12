#include <anton/string_stream.hpp>

namespace anton {
    Input_String_Stream::Input_String_Stream(String_View string): _string(string), _current(_string.chars_begin()) {}
    Input_String_Stream::Input_String_Stream(String string): _string(ANTON_MOV(string)), _current(_string.chars_begin()) {}

    Input_String_Stream::operator bool() const {
        return true;
    }

    i64 Input_String_Stream::read(void*, i64) {
        return 0;
        // TODO: Implement. We currently do not use those, so I just skipped those.
    }

    i64 Input_String_Stream::read(Slice<u8>) {
        return 0;
        // TODO: Implement. We currently do not use those, so I just skipped those.
    }

    char32 Input_String_Stream::peek() {
        if(_current != _string.chars_end()) {
            return *_current;
        } else {
            return eof_char32;
        }
    }

    char32 Input_String_Stream::get() {
        if(_current != _string.chars_end()) {
            char32 c = *_current;
            ++_current;
            return c;
        } else {
            return eof_char32;
        }
    }

    void Input_String_Stream::unget() {
        if(_current != _string.chars_begin()) {
            --_current;
        }
    }

    void Input_String_Stream::seek(Seek_Dir dir, i64 offset) {
        if(dir == Seek_Dir::beg) {
            auto begin = _string.chars_begin();
            _current = UTF8_Char_Iterator{begin.get_underlying_pointer() + offset, offset};
        }
    }

    i64 Input_String_Stream::tell() {
        return _current.get_offset();
    }
} // namespace anton
