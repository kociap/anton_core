#include <anton/string7_stream.hpp>

namespace anton {
    Input_String7_Stream::Input_String7_Stream(String7 string): _string(ANTON_MOV(string)), _current(_string.begin()) {}

    Input_String7_Stream::operator bool() const {
        return true;
    }

    void Input_String7_Stream::read(void*, i64) {
        // TODO: Implement. We currently do not use those, so I just skipped those.
    }

    void Input_String7_Stream::read(Slice<u8>) {
        // TODO: Implement. We currently do not use those, so I just skipped those.
    }

    char32 Input_String7_Stream::peek() {
        if(_current != _string.end()) {
            return *_current;
        } else {
            return eof_char32;
        }
    }

    char32 Input_String7_Stream::get() {
        if(_current != _string.end()) {
            char32 c = *_current;
            ++_current;
            return c;
        } else {
            return eof_char32;
        }
    }

    void Input_String7_Stream::unget() {
        if(_current != _string.begin()) {
            --_current;
        }
    }

    void Input_String7_Stream::seek(Seek_Dir dir, i64 offset) {
        if(dir == Seek_Dir::beg) {
            _current = _string.begin() + offset;
        }
    }

    i64 Input_String7_Stream::tell() {
        return _current - _string.begin();
    }
} // namespace anton
