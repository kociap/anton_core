#pragma once

#include <anton/stream.hpp>
#include <anton/string7.hpp>
#include <anton/string7_view.hpp>

namespace anton {
    struct Input_String7_Stream: public Input_Stream {
    public:
        Input_String7_Stream(String7_View string);
        Input_String7_Stream(String7 string);

        [[nodiscard]] virtual operator bool() const override;

        virtual void read(void* buffer, i64 count) override;
        virtual void read(Slice<u8> buffer) override;
        String7_View read(i64 count);
        [[nodiscard]] virtual char32 peek() override;
        [[nodiscard]] String7_View peek(i64 count);
        virtual char32 get() override;
        virtual void unget() override;
        // The only valid value for offset is a value previously returned by tell.
        // The only allowed value for dir is Seek_Dir::beg. If dir is not Seek_Dir::beg, this function does nothing.
        virtual void seek(anton::Seek_Dir dir, i64 offset) override;
        [[nodiscard]] virtual i64 tell() override;

    private:
        String7 _string;
        char8* _current;
    };
} // namespace anton
