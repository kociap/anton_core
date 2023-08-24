#include <anton/filesystem.hpp>

#include <filesystem>
#include <string_view>

#include <fts.h>
#include <sys/stat.h>

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

    i64 get_last_write_time(String_View const path) {
        String const npath = normalize_path(path);
        struct stat file_stat;
        int const result = lstat(npath.c_str(), &file_stat);
        if(result == -1) {
            return -1;
        }

        // Time is in milliseconds.
        i64 const time = file_stat.st_mtim.tv_nsec / 1000;
        return time;
    }

    bool exists(String_View const path) {
        String const npath = normalize_path(path);
        struct stat file_stat;
        int const result = lstat(npath.c_str(), &file_stat);
        return result == 0;
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

    i64 file_size(String_View const path) {
        String const npath = normalize_path(path);
        struct stat file_stat;
        int const result = lstat(npath.c_str(), &file_stat);
        if(result == -1) {
            return -1;
        }

        return file_stat.st_size;
    }

    // General notes regarding fts:
    // - fts essentially exposes 2 APIs - fts_read and fts_children. The behaviour of those differs.
    //   While using fts_read, fts_path will also include fts_name, but when using fts_children that
    //   is not the case anymore.
    // - fts_children will list only one entry if fts_read has not been called behaving as if it
    //   were called from a directory above and filtering all other entries.
    // - fts can identify directories without stat (FTS_NOSTAT), but it cannot identify files
    //   without stat.

    Array<String> enumerate_directories(String_View const path) {
        String npath = normalize_path(path);
        char* paths[] = {npath.c_str(), nullptr};
        // FTS can identify directories without using stat.
        FTS* const fts = fts_open(paths, FTS_PHYSICAL | FTS_NOSTAT | FTS_NOCHDIR, nullptr);
        // We call fts_read to "descend into the directory" specified to fts_open. Otherwise
        // fts_children will give us a single entry which is the path.
        FTSENT* const read_ent = fts_read(fts);
        ANTON_UNUSED(read_ent);
        FTSENT* ent = fts_children(fts, 0);
        Array<String> directories;
        while(ent != nullptr) {
            if(ent->fts_info == FTS_D) {
                directories.push_back(String(ent->fts_name));
            }

            ent = ent->fts_link;
        }
        fts_close(fts);
        return directories;
    }

    Array<String> enumerate_files(String_View path) {
        String npath = normalize_path(path);
        char* paths[] = {npath.c_str(), nullptr};
        // We need stat because otherwise FTS will not be able to identify files.
        FTS* const fts = fts_open(paths, FTS_PHYSICAL | FTS_NOSTAT | FTS_NOCHDIR, nullptr);
        // We call fts_read to "descend into the directory" specified to fts_open. Otherwise
        // fts_children will give us a single entry which is the path.
        FTSENT* const read_ent = fts_read(fts);
        ANTON_UNUSED(read_ent);
        FTSENT* ent = fts_children(fts, 0);
        Array<String> files;
        while(ent != nullptr) {
            if(ent->fts_info == FTS_F) {
                files.push_back(String(ent->fts_name));
            }

            ent = ent->fts_link;
        }
        fts_close(fts);
        return files;
    }
} // namespace anton::fs
