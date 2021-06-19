#include <anton/filesystem.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>
#include <anton/detail/crt.hpp>
#include <anton/math/math.hpp>
#include <anton/unicode/common.hpp>

#include <Windows.h>

#include <filesystem>
#include <string_view>

namespace anton::fs {
    static Array<char16> string8_to_string16(String_View const string8) {
        i64 const string16_bytes = unicode::convert_utf8_to_utf16(string8.data(), string8.size_bytes(), nullptr);
        // Add 1 to the length for null-terminator.
        Array<char16> string16(1 + string16_bytes / sizeof(char16), 0);
        unicode::convert_utf8_to_utf16(string8.data(), string8.size_bytes(), string16.data());
        return string16;
    }

    static String fs_path_to_string(std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return String{gen_str.data(), (i64)gen_str.size()};
    }

    String normalize_path(String_View const path) {
        std::filesystem::path p(std::string_view(path.data(), path.size_bytes()));
        return fs_path_to_string(p);
    }

    String concat_paths(String_View const lhs, String_View const rhs) {
        std::filesystem::path a(std::string_view(lhs.data(), lhs.size_bytes()));
        std::filesystem::path b(std::string_view(rhs.data(), rhs.size_bytes()));
        a /= b;
        return fs_path_to_string(a);
    }

    String_View remove_filename(String_View const path) {
        for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {path.chars_begin(), i + 1};
            }
        }
        return path;
    }

    String_View remove_extension(String_View const path) {
        auto i = path.chars_end() - 1;
        for(auto const begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return path;
            } else if(c == U'.') {
                return {path.chars_begin(), i};
            }
        }
        return path;
    }

    String_View get_filename(String_View const path) {
        for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\' || c == U':') {
                return {i + 1, path.chars_end()};
            }
        }
        // The path does not contain a directory separator or a volume separator,
        // therefore the entire path is the filename.
        return path;
    }

    String_View get_filename_no_extension(String_View const path) {
        String_View const filename = get_filename(path);
        return remove_extension(filename);
    }

    String_View get_extension(String_View const path) {
        auto i = path.chars_end() - 1;
        for(auto const begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {};
            } else if(c == U'.') {
                return {i, path.chars_end()};
            }
        }
        return {};
    }

    String_View get_directory_name(String_View const path) {
        auto i = path.chars_end() - 1;
        auto const begin = path.chars_begin() - 1;
        for(; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                break;
            }
        }

        if(i == begin) {
            return {};
        }

        String_View directory_name{path.chars_begin(), i};
        // Handle root directory
        if(ends_with(directory_name, ":"_sv)) {
            // The path specifies a drive (identified by the volume separator ":").
            // We return an empty string in this case.
            return {};
        }

        return directory_name;
    }

    String make_relative(String_View path, String_View base_path) {
        std::filesystem::path p(std::string_view(path.data(), path.size_bytes()));
        std::filesystem::path b(std::string_view(base_path.data(), base_path.size_bytes()));
        std::filesystem::path r = std::filesystem::relative(p, b);
        return fs_path_to_string(r);
    }

    i64 get_last_write_time(String_View path) {
        Array<char16> const wpath = string8_to_string16(path);
        // Open file for reading, allow other processes to open for reading, must exist.
        HANDLE const file_handle =
            CreateFileW((wchar_t const*)wpath.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file_handle == INVALID_HANDLE_VALUE) {
            return -1;
        }

        FILETIME last_write_time = {};
        BOOL const got_time = GetFileTime(file_handle, nullptr, nullptr, &last_write_time);
        if(!got_time) {
            return -1;
        }

        i64 last_write_time_64 = (i64)last_write_time.dwHighDateTime << 32 | (i64)last_write_time.dwLowDateTime;
        // Time is reported since 00:00:00 1601-01-01. We want to adjust it so that it starts at 00:00:00 1970-01-01.
        last_write_time_64 -= 0x19DB1DED53E8000LL;
        // Time is reported in 100-nanoseconds. Convert to milliseconds.
        last_write_time_64 /= 10000;
        // Clamp time to be in range 00:00:00 1970-01-01 to present because we don't care about files older than that.
        return math::max(last_write_time_64, (i64)0);
    }

    bool has_filename(String_View const path) {
        String_View const filename = get_filename(path);
        return filename.size_bytes();
    }

    bool has_extension(String_View const path) {
        String_View const ext = get_extension(path);
        return ext.size_bytes();
    }

    bool exists(String_View const path) {
        std::filesystem::path a(std::string_view(path.data(), path.size_bytes()));
        return std::filesystem::exists(a);
    }

    String parent_path(String_View const path) {
        std::filesystem::path a(std::string_view(path.data(), path.size_bytes()));
        return fs_path_to_string(a.parent_path());
    }

    bool copy_file(String_View source, String_View destination, bool overwrite) {
        Array<char16> const source16 = string8_to_string16(source);
        Array<char16> const destination16 = string8_to_string16(destination);
        BOOL cancel = false;
        // We set the NO_BUFFERING flag to support large files,
        // since the microsoft documentation says that it's recommended for large files.
        // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfileexa
        u32 flags = COPY_FILE_NO_BUFFERING;
        if(!overwrite) {
            flags |= COPY_FILE_FAIL_IF_EXISTS;
        }

        bool const result = CopyFileEx((wchar_t const*)source16.data(), (wchar_t const*)destination16.data(), nullptr, nullptr, &cancel, flags);
        return result;
    }

    bool delete_file(String_View path) {
        Array<char16> const path16 = string8_to_string16(path);
        bool const r = DeleteFileW((wchar_t const*)path16.data());
        return r;
    }

    bool delete_directory(String_View path) {
        Array<char16> const path16 = string8_to_string16(path);
        bool const r = RemoveDirectoryW((wchar_t const*)path16.data());
        return r;
    }

    void rename(String_View const from, String_View const to) {
        std::filesystem::path a(std::string_view(from.data(), from.size_bytes()));
        std::filesystem::path b(std::string_view(to.data(), to.size_bytes()));
        std::filesystem::rename(a, b);
    }

    i64 file_size(String_View const p) {
        std::filesystem::path a(std::string_view(p.data(), p.size_bytes()));
        return static_cast<i64>(std::filesystem::file_size(a));
    }

    Array<String> enumerate_directories(String_View path) {
        // We have to append the wildcard character at the end /* to search the directory.
        // TODO: Consider prefixing the path with \\?\ to enable long paths.
        String path_match{reserve, path.size_bytes() + 2};
        path_match += path;
        path_match += u8"/*"_sv;

        Array<char16> const wpath = string8_to_string16(path_match);
        WIN32_FIND_DATA data = {};
        // We use FindExInfoBasic because we don't want the 8.3 name.
        HANDLE find_handle = FindFirstFileExW((wchar_t const*)wpath.data(), FindExInfoBasic, &data, FindExSearchLimitToDirectories, nullptr, 0);
        if(find_handle == INVALID_HANDLE_VALUE) {
            return {};
        }

        Array<String> directories;
        while(true) {
            // The FindExSearchLimitToDirectories flag might be silently ignored
            bool const is_directory = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            if(is_directory) {
                // We don't want to enumerate '.' and '..' directories
                bool const is_current_directory = data.cFileName[0] == '.' && data.cFileName[1] == '\0';
                bool const is_parent_directory = data.cFileName[0] == '.' && data.cFileName[1] == '.' && data.cFileName[2] == '\0';
                if(!is_current_directory && !is_parent_directory) {
                    String directory = String::from_utf16((char16*)data.cFileName);
                    directories.emplace_back(ANTON_MOV(directory));
                }
            }

            bool const r = FindNextFileW(find_handle, &data);
            if(!r) {
                break;
            }
        }

        // The Microsoft documentation states that FindClose might fail.
        // I am unsure what to do in such case.
        FindClose(find_handle);
        return directories;
    }

    Array<String> enumerate_files(String_View path) {
        // We have to append the wildcard character at the end /* to search the directory.
        // TODO: Consider prefixing the path with \\?\ to enable long paths.
        String path_match{reserve, path.size_bytes() + 2};
        path_match += path;
        path_match += u8"/*"_sv;

        Array<char16> const wpath = string8_to_string16(path_match);
        WIN32_FIND_DATA data = {};
        // We use FindExInfoBasic because we don't want the 8.3 name.
        HANDLE find_handle = FindFirstFileExW((wchar_t const*)wpath.data(), FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, 0);
        if(find_handle == INVALID_HANDLE_VALUE) {
            return {};
        }

        Array<String> directories;
        while(true) {
            bool const is_file = !(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            if(is_file) {
                String directory = String::from_utf16((char16*)data.cFileName);
                directories.emplace_back(ANTON_MOV(directory));
            }

            bool const r = FindNextFileW(find_handle, &data);
            if(!r) {
                break;
            }
        }

        // The Microsoft documentation states that FindClose might fail.
        // I am unsure what to do in such case.
        FindClose(find_handle);
        return directories;
    }

    Output_File_Stream::Output_File_Stream() {}

    Output_File_Stream::Output_File_Stream(String const& filename) {
        open(filename);
    }

    Output_File_Stream::Output_File_Stream(String const& filename, Open_Mode const open_mode) {
        open(filename, open_mode);
    }

    Output_File_Stream::Output_File_Stream(Output_File_Stream&& other): _buffer(other._buffer) {
        other._buffer = nullptr;
    }

    Output_File_Stream& Output_File_Stream::operator=(Output_File_Stream&& other) {
        swap(_buffer, other._buffer);
        return *this;
    }

    Output_File_Stream::~Output_File_Stream() {
        close();
    }

    Output_File_Stream::operator bool() const {
        return is_open();
    }

    bool Output_File_Stream::open(String const& filename) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        _buffer = fopen(filename.data(), "wb");
        return _buffer != nullptr;
    }

    bool Output_File_Stream::open(String const& filename, Open_Mode const open_mode) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        // TODO: implement open modes.
        _buffer = fopen(filename.data(), "w");
        return _buffer != nullptr;
    }

    void Output_File_Stream::close() {
        if(_buffer) {
            fclose((FILE*)_buffer);
            _buffer = nullptr;
        }
    }

    bool Output_File_Stream::is_open() const {
        return _buffer != nullptr;
    }

    void Output_File_Stream::flush() {
        ANTON_ASSERT(_buffer, "Attempting to flush the stream, but no file has been opened.");
        fflush((FILE*)_buffer);
    }

    void Output_File_Stream::write(void const* buffer, i64 count) {
        ANTON_ASSERT(_buffer, "Attempting to write to the stream, but no file has been opened.");
        fwrite(buffer, count, 1, (FILE*)_buffer);
    }

    void Output_File_Stream::write(Slice<u8 const> const buffer) {
        ANTON_ASSERT(_buffer, "Attempting to write to the stream, but no file has been opened.");
        fwrite(buffer.data(), buffer.size(), 1, (FILE*)_buffer);
    }

    void Output_File_Stream::write(String_View const buffer) {
        ANTON_ASSERT(_buffer, "Attempting to write to the stream, but no file has been opened.");
        fwrite(buffer.data(), buffer.size_bytes(), 1, (FILE*)_buffer);
    }

    void Output_File_Stream::put(char32 c) {
        ANTON_ASSERT(_buffer, "Attempting to write to the stream, but no file has been opened.");
        fputc(c, (FILE*)_buffer);
    }

    void Output_File_Stream::seek(Seek_Dir dir, i64 offset) {
        ANTON_ASSERT(_buffer, "Attempting to seek the stream, but no file has been opened.");
        fseek((FILE*)_buffer, (long)offset, (int)(dir));
    }

    i64 Output_File_Stream::tell() {
        ANTON_ASSERT(_buffer, "Attempting to tell the stream, but no file has been opened.");
        return ftell((FILE*)_buffer);
    }

    Input_File_Stream::Input_File_Stream(): _open_mode((Open_Mode)0) {}

    Input_File_Stream::Input_File_Stream(String const& filename): _open_mode((Open_Mode)0) {
        open(filename);
    }

    Input_File_Stream::Input_File_Stream(String const& filename, Open_Mode const open_mode): _open_mode(open_mode) {
        open(filename, open_mode);
    }

    Input_File_Stream::Input_File_Stream(Input_File_Stream&& other): _buffer(other._buffer) {
        other._buffer = nullptr;
    }

    Input_File_Stream& Input_File_Stream::operator=(Input_File_Stream&& other) {
        swap(_buffer, other._buffer);
        return *this;
    }

    Input_File_Stream::~Input_File_Stream() {
        close();
    }

    Input_File_Stream::operator bool() const {
        return is_open();
    }

    bool Input_File_Stream::open(String const& filename) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        _buffer = fopen(filename.data(), "rb");
        return _buffer != nullptr;
    }

    bool Input_File_Stream::open(String const& filename, Open_Mode const open_mode) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        // TODO: implement open modes.
        _buffer = fopen(filename.data(), open_mode != Open_Mode::windows_translate_newline ? "rb" : "r");
        return _buffer != nullptr;
    }

    void Input_File_Stream::close() {
        if(_buffer) {
            fclose((FILE*)_buffer);
            _buffer = nullptr;
        }
    }

    bool Input_File_Stream::is_open() const {
        return _buffer != nullptr;
    }

    void Input_File_Stream::read(void* buffer, i64 count) {
        ANTON_ASSERT(_buffer, "Attempting to read from the stream, but no file has been opened.");
        fread(buffer, count, 1, (FILE*)_buffer);
    }

    void Input_File_Stream::read(Slice<u8> const buffer) {
        ANTON_ASSERT(_buffer, "Attempting to read from the stream, but no file has been opened.");
        fread(buffer.data(), buffer.size(), 1, (FILE*)_buffer);
    }

    char32 Input_File_Stream::peek() {
        char32 const c = get();
        // Do not call unget when the next char is eof because that will push
        // us back in the stream to the character before eof (next call to get will
        // return the character right before eof).
        if(c != eof_char32) {
            unget();
        }
        return c;
    }

    char32 Input_File_Stream::get() {
        ANTON_ASSERT(_buffer, "Attempting to read from the stream, but no file has been opened.");
        return fgetc((FILE*)_buffer);
    }

    void Input_File_Stream::unget() {
        ANTON_ASSERT(_buffer, "Attempting to unget to the stream, but no file has been opened.");
        // We can't guarantee reliable unget when the file is in non-binary mode.
        bool const text_mode = (u32)_open_mode & (u32)Open_Mode::windows_translate_newline;
        if(!text_mode) {
            i64 n = tell();
            seek(Seek_Dir::beg, n - 1);
        }
    }

    void Input_File_Stream::seek(Seek_Dir dir, i64 offset) {
        ANTON_ASSERT(_buffer, "Attempting to seek in the stream, but no file has been opened.");
        fseek((FILE*)_buffer, (long)offset, (int)(dir));
    }

    i64 Input_File_Stream::tell() {
        ANTON_ASSERT(_buffer, "Attempting to tell the stream, but no file has been opened.");
        return ftell((FILE*)_buffer);
    }

    bool Input_File_Stream::eof() const {
        ANTON_ASSERT(_buffer, "Attempting to get error state from the stream, but no file has been opened.");
        return feof((FILE*)_buffer);
    }

    bool Input_File_Stream::error() const {
        ANTON_ASSERT(_buffer, "Attempting to get error state the stream, but no file has been opened.");
        return ferror((FILE*)_buffer);
    }
} // namespace anton::fs
