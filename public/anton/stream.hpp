#pragma once

#include <anton/slice.hpp>
#include <anton/string_view.hpp>
#include <anton/types.hpp>

namespace anton {
    constexpr char32 eof_char32 = (char32)-1;

    enum class Seek_Dir {
        beg,
        cur,
        end,
    };

    class Output_Stream {
    public:
        virtual ~Output_Stream() {}

        [[nodiscard]] virtual operator bool() const = 0;

        virtual void flush() = 0;
        virtual void write(void const* buffer, i64 count) = 0;
        virtual void write(Slice<u8 const> buffer) = 0;
        virtual void write(String_View buffer) = 0;
        virtual void put(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };

    class Input_Stream {
    public:
        virtual ~Input_Stream() {}

        [[nodiscard]] virtual operator bool() const = 0;

        virtual void read(void* buffer, i64 count) = 0;
        virtual void read(Slice<u8> buffer) = 0;
        virtual char32 peek() = 0;
        virtual char32 get() = 0;
        virtual void unget(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };
} // namespace anton
