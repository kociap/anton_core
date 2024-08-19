#include <anton/filesystem.hpp>

#include <anton/detail/crt.hpp>

namespace anton::fs {
  String_View remove_filename(String_View const path)
  {
    for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1;
        i != begin; --i) {
      char32 const c = *i;
      if(c == U'/' || c == U'\\') {
        return {path.chars_begin(), i + 1};
      }
    }
    return path;
  }

  String_View remove_extension(String_View const path)
  {
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

  String_View get_filename(String_View const path)
  {
    for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1;
        i != begin; --i) {
      char32 const c = *i;
      if(c == U'/' || c == U'\\' || c == U':') {
        return {i + 1, path.chars_end()};
      }
    }
    // The path does not contain a directory separator or a volume separator,
    // therefore the entire path is the filename.
    return path;
  }

  String_View get_filename_no_extension(String_View const path)
  {
    String_View const filename = get_filename(path);
    return remove_extension(filename);
  }

  String_View get_extension(String_View const path)
  {
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

  String_View get_directory_name(String_View const path)
  {
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

  bool has_filename(String_View const path)
  {
    String_View const filename = get_filename(path);
    return filename.size_bytes();
  }

  bool has_extension(String_View const path)
  {
    String_View const ext = get_extension(path);
    return ext.size_bytes();
  }

  Output_File_Stream::Output_File_Stream() {}

  Output_File_Stream::Output_File_Stream(String const& filename)
  {
    open(filename);
  }

  Output_File_Stream::Output_File_Stream(String const& filename,
                                         Open_Mode const open_mode)
  {
    open(filename, open_mode);
  }

  Output_File_Stream::Output_File_Stream(Output_File_Stream&& other)
    : _buffer(other._buffer)
  {
    other._buffer = nullptr;
  }

  Output_File_Stream& Output_File_Stream::operator=(Output_File_Stream&& other)
  {
    swap(_buffer, other._buffer);
    return *this;
  }

  Output_File_Stream::~Output_File_Stream()
  {
    close();
  }

  Output_File_Stream::operator bool() const
  {
    return is_open();
  }

  bool Output_File_Stream::open(String const& filename)
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
    }

    _buffer = fopen(filename.data(), "wb");
    return _buffer != nullptr;
  }

  bool Output_File_Stream::open(String const& filename,
                                Open_Mode const open_mode)
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
    }

    // TODO: implement open modes.
    _buffer = fopen(filename.data(), "w");
    return _buffer != nullptr;
  }

  void Output_File_Stream::close()
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
      _buffer = nullptr;
    }
  }

  bool Output_File_Stream::is_open() const
  {
    return _buffer != nullptr;
  }

  void Output_File_Stream::flush()
  {
    ANTON_ASSERT(
      _buffer, "Attempting to flush the stream, but no file has been opened.");
    fflush((FILE*)_buffer);
  }

  void Output_File_Stream::write(void const* buffer, i64 count)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to write to the stream, but no file has been opened.");
    fwrite(buffer, count, 1, (FILE*)_buffer);
  }

  void Output_File_Stream::write(Slice<u8 const> const buffer)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to write to the stream, but no file has been opened.");
    fwrite(buffer.data(), buffer.size(), 1, (FILE*)_buffer);
  }

  void Output_File_Stream::write(String_View const buffer)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to write to the stream, but no file has been opened.");
    fwrite(buffer.data(), buffer.size_bytes(), 1, (FILE*)_buffer);
  }

  void Output_File_Stream::put(char32 c)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to write to the stream, but no file has been opened.");
    fputc(c, (FILE*)_buffer);
  }

  void Output_File_Stream::seek(Seek_Dir dir, i64 offset)
  {
    ANTON_ASSERT(_buffer,
                 "Attempting to seek the stream, but no file has been opened.");
    fseek((FILE*)_buffer, (long)offset, (int)(dir));
  }

  i64 Output_File_Stream::tell()
  {
    ANTON_ASSERT(_buffer,
                 "Attempting to tell the stream, but no file has been opened.");
    return ftell((FILE*)_buffer);
  }

  Input_File_Stream::Input_File_Stream(): _open_mode((Open_Mode)0) {}

  Input_File_Stream::Input_File_Stream(String const& filename)
    : _open_mode((Open_Mode)0)
  {
    open(filename);
  }

  Input_File_Stream::Input_File_Stream(String const& filename,
                                       Open_Mode const open_mode)
    : _open_mode(open_mode)
  {
    open(filename, open_mode);
  }

  Input_File_Stream::Input_File_Stream(Input_File_Stream&& other)
    : _buffer(other._buffer)
  {
    other._buffer = nullptr;
  }

  Input_File_Stream& Input_File_Stream::operator=(Input_File_Stream&& other)
  {
    swap(_buffer, other._buffer);
    return *this;
  }

  Input_File_Stream::~Input_File_Stream()
  {
    close();
  }

  Input_File_Stream::operator bool() const
  {
    return is_open();
  }

  bool Input_File_Stream::open(String const& filename)
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
    }

    _buffer = fopen(filename.data(), "rb");
    return _buffer != nullptr;
  }

  bool Input_File_Stream::open(String const& filename,
                               Open_Mode const open_mode)
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
    }

    // TODO: implement open modes.
    _buffer =
      fopen(filename.data(),
            open_mode != Open_Mode::windows_translate_newline ? "rb" : "r");
    return _buffer != nullptr;
  }

  void Input_File_Stream::close()
  {
    if(_buffer) {
      fclose((FILE*)_buffer);
      _buffer = nullptr;
    }
  }

  bool Input_File_Stream::is_open() const
  {
    return _buffer != nullptr;
  }

  i64 Input_File_Stream::read(void* buffer, i64 count)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to read from the stream, but no file has been opened.");
    return fread(buffer, 1, count, (FILE*)_buffer);
  }

  i64 Input_File_Stream::read(Slice<u8> const buffer)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to read from the stream, but no file has been opened.");
    return fread(buffer.data(), 1, buffer.size(), (FILE*)_buffer);
  }

  char32 Input_File_Stream::peek()
  {
    char32 const c = get();
    // Do not call unget when the next char is eof because that will push
    // us back in the stream to the character before eof (next call to get will
    // return the character right before eof).
    if(c != eof_char32) {
      unget();
    }
    return c;
  }

  char32 Input_File_Stream::get()
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to read from the stream, but no file has been opened.");
    return fgetc((FILE*)_buffer);
  }

  void Input_File_Stream::unget()
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to unget to the stream, but no file has been opened.");
    // We can't guarantee reliable unget when the file is in non-binary mode.
    bool const text_mode =
      (u32)_open_mode & (u32)Open_Mode::windows_translate_newline;
    if(!text_mode) {
      i64 n = tell();
      seek(Seek_Dir::beg, n - 1);
    }
  }

  void Input_File_Stream::seek(Seek_Dir dir, i64 offset)
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to seek in the stream, but no file has been opened.");
    fseek((FILE*)_buffer, (long)offset, (int)(dir));
  }

  i64 Input_File_Stream::tell()
  {
    ANTON_ASSERT(_buffer,
                 "Attempting to tell the stream, but no file has been opened.");
    return ftell((FILE*)_buffer);
  }

  bool Input_File_Stream::eof() const
  {
    ANTON_ASSERT(_buffer,
                 "Attempting to get error state from the stream, but no file "
                 "has been opened.");
    return feof((FILE*)_buffer);
  }

  bool Input_File_Stream::error() const
  {
    ANTON_ASSERT(
      _buffer,
      "Attempting to get error state the stream, but no file has been opened.");
    return ferror((FILE*)_buffer);
  }
} // namespace anton::fs
