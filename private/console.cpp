#include <anton/console.hpp>

#include <stdio.h>

#if defined(_WIN64)
#    include <fcntl.h>
#    include <io.h>
#endif

namespace anton {
    STDOUT_Stream::operator bool() const {
        return true;
    }

    void STDOUT_Stream::flush() {
        fflush(stdout);
    }

    void STDOUT_Stream::write(void const* buffer, i64 count) {
        fwrite(buffer, count, 1, stdout);
    }

    void STDOUT_Stream::write(Slice<u8 const> const buffer) {
        fwrite(buffer.data(), buffer.size(), 1, stdout);
    }

    void STDOUT_Stream::write(String_View const buffer) {
        fwrite(buffer.data(), buffer.size_bytes(), 1, stdout);
    }

    void STDOUT_Stream::put(char32 c) {
        fputc(c, stdout);
    }

    void STDOUT_Stream::seek(Seek_Dir dir, i64 offset) {
        fseek(stdout, (long)offset, (int)(dir));
    }

    i64 STDOUT_Stream::tell() {
        return ftell(stdout);
    }

    STDERR_Stream::operator bool() const {
        return true;
    }

    void STDERR_Stream::flush() {
        fflush(stderr);
    }

    void STDERR_Stream::write(void const* buffer, i64 count) {
        fwrite(buffer, count, 1, stderr);
    }

    void STDERR_Stream::write(Slice<u8 const> const buffer) {
        fwrite(buffer.data(), buffer.size(), 1, stderr);
    }

    void STDERR_Stream::write(String_View const buffer) {
        fwrite(buffer.data(), buffer.size_bytes(), 1, stderr);
    }

    void STDERR_Stream::put(char32 c) {
        fputc(c, stderr);
    }

    void STDERR_Stream::seek(Seek_Dir dir, i64 offset) {
        fseek(stderr, (long)offset, (int)(dir));
    }

    i64 STDERR_Stream::tell() {
        return ftell(stderr);
    }

    bool set_stdin_binary() {
#if defined(_WIN64)
        return _setmode(_fileno(stdin), _O_BINARY) != -1;
#else
        return true;
#endif
    }

    bool set_stdout_binary() {
#if defined(_WIN64)
        return _setmode(_fileno(stdout), _O_BINARY) != -1;
#else
        return true;
#endif
    }
} // namespace anton