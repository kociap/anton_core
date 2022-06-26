#include <anton/console.hpp>

#include <stdio.h>

#if defined(_WIN64)
#    include <fcntl.h>
#    include <io.h>
#endif

namespace anton {
    Console_Output::~Console_Output() {}

    Console_Output::operator bool() const {
        return true;
    }

    void Console_Output::flush() {
        fflush(stdout);
    }

    void Console_Output::write(void const* buffer, i64 count) {
        fwrite(buffer, count, 1, stdout);
    }

    void Console_Output::write(Slice<u8 const> const buffer) {
        fwrite(buffer.data(), buffer.size(), 1, stdout);
    }

    void Console_Output::write(String_View const buffer) {
        fwrite(buffer.data(), buffer.size_bytes(), 1, stdout);
    }

    void Console_Output::put(char32 c) {
        fputc(c, stdout);
    }

    void Console_Output::seek(Seek_Dir dir, i64 offset) {
        fseek(stdout, (long)offset, (int)(dir));
    }

    i64 Console_Output::tell() {
        return ftell(stdout);
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