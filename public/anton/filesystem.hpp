#pragma once

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
    [[nodiscard]] String_View get_filename(String_View path);
    [[nodiscard]] String_View get_filename_no_extension(String_View path);
    [[nodiscard]] String_View get_extension(String_View path);

    [[nodiscard]] String parent_path(String_View const path);

    // get_directory_name
    // Does not support paths using "file:".
    //
    [[nodiscard]] String_View get_directory_name(String_View path);

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

    void rename(String_View from, String_View to);

    // file_size
    // Obtain size of a file in bytes.
    //
    i64 file_size(String_View path);

    enum class Open_Mode : u32 {
        // Has effect only on Windows. Makes all reading operations translate the \n\r sequences into \n.
        // Opposite of std::ios_base::open_mode::binary.
        windows_translate_newline = 1,
    };

    class Output_File_Stream: public Output_Stream {
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

    class Input_File_Stream: public Input_Stream {
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