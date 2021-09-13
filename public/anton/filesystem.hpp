#pragma once

#include <anton/array.hpp>
#include <anton/stream.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>

namespace anton::fs {
    // normalize_path
    //
    [[nodiscard]] String normalize_path(String_View path);

    // concat_paths
    // Concatenate paths with separator.
    //
    [[nodiscard]] String concat_paths(String_View path1, String_View path2);

    [[nodiscard]] String_View remove_filename(String_View path);
    [[nodiscard]] String_View remove_extension(String_View path);

    // get_filename
    // Returns the filename and extension of the specified path.
    // Does not support paths using "file:". Does not support UNC.
    //
    // Parameters:
    // path - the path from which to get the filename and extension.
    //
    // Returns:
    // A string containing the characters after the last directory or volume separator.
    //
    [[nodiscard]] String_View get_filename(String_View path);

    // get_filename_no_extension
    // Returns the filename of the specified path without the extension.
    // Does not support paths using "file:". Does not support UNC.
    //
    // Parameters:
    // path - the path from which to get the filename.
    //
    // Returns:
    // A string containing the characters after the last directory or volume separator
    // exlcuding the last period and the characters following it.
    //
    [[nodiscard]] String_View get_filename_no_extension(String_View path);

    // get_extension
    // Returns the extension including the period "." of the specified path.
    //
    // Parameters:
    // path - the path from which to get the extension.
    //
    // Returns:
    // The extension including the period "." of the specified path or
    // an empty view if path does not have an extension.
    //
    [[nodiscard]] String_View get_extension(String_View path);

    [[nodiscard]] String parent_path(String_View const path);

    // get_directory_name
    // Returns the directory information for the specified path.
    // Does not support paths using "file:". Does not support UNC.
    //
    // Parameters:
    // path - the path of a file or directory.
    //
    // Returns:
    // Returns a string consisting of all characters in path up to, but not including,
    // the last directory separator. If the path consists of a root directory, e.g. "C:/",
    // the returned string is empty.
    //
    [[nodiscard]] String_View get_directory_name(String_View path);

    // make_relative
    //
    [[nodiscard]] String make_relative(String_View path, String_View base_path);

    // get_last_write_time
    // Gets the last write time of a file identified by path.
    // The file must exist and it must be possible to open it for reading.
    //
    // Returns:
    // Time of the last modification of the file in milliseconds since 00:00:00 1970-01-01
    // or -1 if the function failed to retrieve the information.
    //
    [[nodiscard]] i64 get_last_write_time(String_View path);

    [[nodiscard]] bool has_extension(String_View path);
    [[nodiscard]] bool has_filename(String_View path);
    [[nodiscard]] bool exists(String_View path);

    // create_directory
    // Creates a new directory identified by path.
    // If the directory already exists, the function will fail.
    // Only the final directory will be created. Intermediate directories
    // will not be created and if they do not exist, the function will fail.
    //
    // Parameters:
    // path - the path of the directory to be created.
    //
    // Returns:
    // true if the directory has been successfully created.
    // false if the directory could not be created. Possible reasons include:
    // - The directory already exists.
    // - The intermediate directories do not exist.
    //
    bool create_directory(String_View path);

    struct Copy_Options {
        // Whether to overwrite the directories if they already exist.
        // If false, the existing files and directories are kept.
        bool overwrite;
        // Whether to copy all subdirectories.
        bool recursive;
        // Whether to copy only the directory structure.
        // If false, all files within the directories are also copied.
        bool only_directories;
    };

    // copy_file
    // Copies and existing file to a new file.
    // If the file does not exist, this function will return false.
    //
    // Parameters:
    //      source - the name of an existing file.
    // destination - the new name of the copied file.
    //   overwrite - whether to overwrite the file if it already extists.
    //               If this parameter is false, the existing file is kept
    //               and no copies are performed.
    //
    // Returns:
    // true if the file has been copied, false otherwise.
    //
    bool copy_file(String_View source, String_View destination, bool overwrite);

    // delete_file
    // Deletes an existing file.
    // If the file does not exist, this function will return false.
    //
    // Parameters:
    // path - the name of the file to be deleted.
    //
    // Returns:
    // true if the file has been deleted, false otherwise.
    //
    bool delete_file(String_View path);

    // delete_directory
    // Deletes an existing empty directory.
    // If the directory does not exist, this function will return false.
    //
    // Parameters:
    // path - the name of the empty directory to be deleted.
    //
    // Returns:
    // true if the file has been deleted, false otherwise.
    //
    bool delete_directory(String_View path);

    void rename(String_View from, String_View to);

    // file_size
    // Obtain size of a file in bytes.
    //
    [[nodiscard]] i64 file_size(String_View path);

    // enumerate_directories
    // Enumerates all subdirectories within the directory identified by path.
    // Does not include the current directory (".") or the parent directory ("..").
    //
    // Parameters:
    // path - directory to enumerate. The path must not end in a slash ('/' or '\\').
    //
    // Returns:
    // Array of subdirectory names relative to path.
    //
    [[nodiscard]] Array<String> enumerate_directories(String_View path);

    // enumerate_files
    // Enumerates all files within the directory identified by path.
    //
    // Parameters:
    // path - directory to enumerate. The path must not end in a slash ('/' or '\\').
    //
    // Returns:
    // Array of file names relative to path.
    //
    [[nodiscard]] Array<String> enumerate_files(String_View path);

    enum struct Open_Mode : u32 {
        // Has effect only on Windows. Makes all reading operations translate the \n\r sequences into \n.
        // Opposite of std::ios_base::open_mode::binary.
        windows_translate_newline = 1,
    };

    struct Output_File_Stream: public Output_Stream {
    public:
        Output_File_Stream();
        explicit Output_File_Stream(String const& filename);
        explicit Output_File_Stream(String const& filename, Open_Mode open_mode);
        Output_File_Stream(Output_File_Stream const&) = delete;
        Output_File_Stream(Output_File_Stream&&);
        Output_File_Stream& operator=(Output_File_Stream const&) = delete;
        Output_File_Stream& operator=(Output_File_Stream&&);
        virtual ~Output_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(String const& filename);
        bool open(String const& filename, Open_Mode open_mode);
        void close();
        [[nodiscard]] bool is_open() const;

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void write(Slice<u8 const> buffer) override;
        virtual void write(String_View buffer) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        [[nodiscard]] virtual i64 tell() override;

    private:
        void* _buffer = nullptr;
    };

    struct Input_File_Stream: public Input_Stream {
    public:
        Input_File_Stream();
        explicit Input_File_Stream(String const& filename);
        explicit Input_File_Stream(String const& filename, Open_Mode open_mode);
        Input_File_Stream(Input_File_Stream const&) = delete;
        Input_File_Stream(Input_File_Stream&&);
        Input_File_Stream& operator=(Input_File_Stream const&) = delete;
        Input_File_Stream& operator=(Input_File_Stream&&);
        virtual ~Input_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(String const& filename);
        bool open(String const& filename, Open_Mode open_mode);
        void close();
        [[nodiscard]] bool is_open() const;

        virtual void read(void* buffer, i64 count) override;
        virtual void read(Slice<u8> buffer) override;
        [[nodiscard]] virtual char32 peek() override;
        virtual char32 get() override;

        // unget
        // Steps 1 character back in the stream.
        // If the stream has been opened in text mode, this function has no effect.
        //
        virtual void unget() override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        [[nodiscard]] virtual i64 tell() override;

        // eof
        // Checks whether the stream has reached end-of-file.
        //
        bool eof() const;

        // error
        // Checks whether the stream has encountered any errors.
        //
        bool error() const;

    private:
        void* _buffer = nullptr;
        Open_Mode _open_mode;
    };
} // namespace anton::fs
