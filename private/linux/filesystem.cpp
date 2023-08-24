#include <anton/filesystem.hpp>

#include <filesystem>
#include <string_view>

namespace anton::fs {
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
        // TODO: Implement
        return 0;
    }

    bool exists(String_View const path) {
        // TODO: Implement
        return false;
    }

    String parent_path(String_View const path) {
        std::filesystem::path a(std::string_view(path.data(), path.size_bytes()));
        return fs_path_to_string(a.parent_path());
    }

    bool create_directory(String_View const path) {
        // TODO: Implement
        return false;
    }

    bool copy_file(String_View source, String_View destination, bool overwrite) {
        // TODO: Implement
        return false;
    }

    bool delete_file(String_View path) {
        // TODO: Implement
        return false;
    }

    bool delete_directory(String_View path) {
        // TODO: Implement
        return false;
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
        // TODO: Implement
        return {};
    }

    Array<String> enumerate_files(String_View path) {
        // TODO: Implement
        return {};
    }
} // namespace anton::fs
