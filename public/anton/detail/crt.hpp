#pragma once

#if defined(_WIN64)
  #define ANTON_CRT_NOEXCEPT
  #define ANTON_CRT_IMPORT __declspec(dllimport)
  #define size_t unsigned long long
#else
  #define ANTON_CRT_NOEXCEPT noexcept
  #define ANTON_CRT_IMPORT
  #define size_t unsigned long int
#endif

// C Runtime Forward Declarations

extern "C" {
// stddef.h

#ifndef offsetof
  #define offsetof(s, m) __builtin_offsetof(s, m)
#endif

// time.h

struct timespec;

// string.h
// memset, memmove, memcpy, strlen don't use dllimport on win.

void* memset(void* dest, int value, size_t count) ANTON_CRT_NOEXCEPT;
void* memcpy(void* dest, void const* src, size_t count) ANTON_CRT_NOEXCEPT;
void* memmove(void* dest, void const* src, size_t count) ANTON_CRT_NOEXCEPT;
int memcmp(void const* lhs, void const* rhs, size_t count) ANTON_CRT_NOEXCEPT;

size_t strlen(char const* string) ANTON_CRT_NOEXCEPT;

// Aligned allocation functions
#if defined(_WIN64)
ANTON_CRT_IMPORT void* _aligned_malloc(size_t size,
                                       size_t alignment); // malloc.h
ANTON_CRT_IMPORT void _aligned_free(void* memory); // malloc.h

  #define ANTON_CRT_ALIGNED_ALLOC(size, alignment) \
    _aligned_malloc(size, alignment)
  #define ANTON_CRT_ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
// aligned_alloc is C11
void* aligned_alloc(size_t alignment, size_t size) ANTON_CRT_NOEXCEPT;
void free(void* ptr) ANTON_CRT_NOEXCEPT;

  #define ANTON_CRT_ALIGNED_ALLOC(size, alignment) \
    aligned_alloc(alignment, size)
  #define ANTON_CRT_ALIGNED_FREE(ptr) free(ptr)
#endif

// stdlib.h

ANTON_CRT_IMPORT float strtof(char const*, char**) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT double strtod(char const*, char**) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT long long strtoll(char const*, char**,
                                   int base) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT unsigned long long strtoull(char const*, char**,
                                             int base) ANTON_CRT_NOEXCEPT;

// stdio.h

#if defined(_WIN64)
  #ifndef _FILE_DEFINED
    #define _FILE_DEFINED

typedef struct _iobuf {
  void* _Placeholder;
} FILE;
  #endif

ANTON_CRT_IMPORT FILE* __acrt_iob_func(unsigned _Ix);

  #define stdin (__acrt_iob_func(0))
  #define stdout (__acrt_iob_func(1))
  #define stderr (__acrt_iob_func(2))
#else
  #ifndef __FILE_defined
    #define __FILE_defined 1
typedef struct _IO_FILE FILE;
  #endif

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF (-1)

ANTON_CRT_IMPORT int fprintf(FILE* stream, char const* format, ...);
ANTON_CRT_IMPORT FILE* fopen(char const* filename, char const* modes);
ANTON_CRT_IMPORT FILE* freopen(char const* filename, char const* mode,
                               FILE* stream);
ANTON_CRT_IMPORT int fclose(FILE* stream);
ANTON_CRT_IMPORT int fflush(FILE* stream);
ANTON_CRT_IMPORT void setbuf(FILE* stream, char* buffer) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT int setvbuf(FILE* stream, char* buffer, int mode,
                             size_t size) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT size_t fread(void* buffer, size_t size, size_t count,
                              FILE* stream);
ANTON_CRT_IMPORT size_t fwrite(void const* buffer, size_t size, size_t count,
                               FILE* stream);
ANTON_CRT_IMPORT int fgetc(FILE* stream);
ANTON_CRT_IMPORT char* fgets(char*, int count, FILE* stream);
ANTON_CRT_IMPORT char* gets(char* string);
ANTON_CRT_IMPORT int getchar(void);
ANTON_CRT_IMPORT int fputc(int ch, FILE* stream);
ANTON_CRT_IMPORT int fputs(char const* string, FILE* stream);
ANTON_CRT_IMPORT int puts(char const* string);
ANTON_CRT_IMPORT int putchar(int ch);
ANTON_CRT_IMPORT int ungetc(int ch, FILE* stream);
ANTON_CRT_IMPORT long ftell(FILE* stream);
ANTON_CRT_IMPORT int fseek(FILE* stream, long offset, int origin);
ANTON_CRT_IMPORT void rewind(FILE* stream);
ANTON_CRT_IMPORT int ferror(FILE* stream) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT int feof(FILE* stream) ANTON_CRT_NOEXCEPT;
ANTON_CRT_IMPORT void clearerr(FILE* stream) ANTON_CRT_NOEXCEPT;
}

// C++ Runtime Forward Declarations

#if defined(_WIN64)
  // The following placement functions are defined inline in vcruntime_new.h
  #ifndef __PLACEMENT_NEW_INLINE
    #define __PLACEMENT_NEW_INLINE
[[nodiscard]] inline void* operator new(size_t _Size, void* _Where) noexcept
{
  (void)_Size;
  return _Where;
}

inline void operator delete(void*, void*) noexcept
{
  return;
}
  #endif // !__PLACEMENT_NEW_INLINE
#else
void* operator new(size_t size, void*) noexcept;
void operator delete(void* ptr, void* place) noexcept;
#endif

#undef ANTON_CRT_NOEXCEPT
#undef ANTON_CRT_IMPORT
#undef size_t
