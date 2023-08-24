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
    [[nodiscard]] static Array<char16> string8_to_string16(String_View const string8) {
        i64 const string16_bytes = unicode::convert_utf8_to_utf16(string8.data(), string8.size_bytes(), nullptr);
        // Add 1 to the length for null-terminator.
        Array<char16> string16(1 + string16_bytes / sizeof(char16), 0);
        unicode::convert_utf8_to_utf16(string8.data(), string8.size_bytes(), string16.data());
        return string16;
    }

    [[nodiscard]] static String fs_path_to_string(std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return String{gen_str.data(), (i64)gen_str.size()};
    }

    [[nodiscard]] static String fs_path_to_string(Memory_Allocator* allocator, std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return String{gen_str.data(), (i64)gen_str.size(), allocator};
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

    String concat_paths(Memory_Allocator* allocator, String_View const lhs, String_View const rhs) {
        std::filesystem::path a(std::string_view(lhs.data(), lhs.size_bytes()));
        std::filesystem::path b(std::string_view(rhs.data(), rhs.size_bytes()));
        a /= b;
        return fs_path_to_string(allocator, a);
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

    bool exists(String_View const path) {
        Array<char16> const path16 = string8_to_string16(path);
        DWORD const result = GetFileAttributesW((wchar_t const*)path16.data());
        if(result != INVALID_FILE_ATTRIBUTES) {
            return true;
        }

        // If GetFileAttributes returns INVALID_FILE_ATTRIBUTES, then it means the
        // function could not access the attributes for some reason.
        // To figure out whether the file actually does not exist, we must check
        // the error code returned by GetLastError.
        DWORD const error = GetLastError();
        return error != ERROR_PATH_NOT_FOUND && error != ERROR_FILE_NOT_FOUND && error != ERROR_INVALID_NAME && error != ERROR_BAD_NETPATH;
    }

    String parent_path(String_View const path) {
        std::filesystem::path a(std::string_view(path.data(), path.size_bytes()));
        return fs_path_to_string(a.parent_path());
    }

    bool create_directory(String_View const path) {
        Array<char16> const path16 = string8_to_string16(path);
        return CreateDirectory((wchar_t const*)path16.data(), NULL);
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
} // namespace anton::fs
