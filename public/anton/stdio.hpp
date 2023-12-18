#pragma once

#include <anton/stream.hpp>
#include <anton/string7_view.hpp>

namespace anton {
    // print
    void print(String_View string);
    void print(String7_View string);

    // STDOUT_Stream
    // Stateless stream representing the STDOUT. Multiple instances may coexist.
    //
    struct STDOUT_Stream: public Output_Stream {
    public:
        virtual ~STDOUT_Stream() override = default;

        [[nodiscard]] virtual operator bool() const override;

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void write(Slice<u8 const> buffer) override;
        virtual void write(String_View buffer) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;
    };

    // STDERR_Stream
    // Stateless stream representing the STDERR. Multiple instances may coexist.
    //
    struct STDERR_Stream: public Output_Stream {
    public:
        virtual ~STDERR_Stream() override = default;

        [[nodiscard]] virtual operator bool() const override;

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void write(Slice<u8 const> buffer) override;
        virtual void write(String_View buffer) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;
    };

    // set_stdin_binary
    // Set stdin to binary mode preventing \r\n sequences
    // from being translated into \n.
    //
    // This function has effect only on Windows. On Linux
    // there is no difference between binary and text mode.
    //
    // Returns:
    // (Windows) true if the mode has been successfully changed.
    // (Linux) always true.
    //
    bool set_stdin_binary();

    // set_stdout_binary
    // Set stdout to binary mode preventing \n from being
    // translated into \r\n sequences.
    //
    // This function has effect only on Windows. On Linux
    // there is no difference between binary and text mode.
    //
    // Returns:
    // (Windows) true if the mode has been successfully changed.
    // (Linux) always true.
    //
    bool set_stdout_binary();
} // namespace anton
