#pragma once

#include <anton/stream.hpp>

namespace anton {
    class Console_Output: public Output_Stream {
    public:
        virtual ~Console_Output() override;

        [[nodiscard]] virtual operator bool() const override;

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void write(Slice<u8 const> buffer) override;
        virtual void write(String_View buffer) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;
    };
} // namespace anton
