#include <anton/memory.hpp>

#include <anton/assert.hpp>

namespace anton {
  void* allocate(i64 size, i64 alignment)
  {
    ANTON_VERIFY(alignment > 0 && !(alignment & (alignment - 1)),
                 "alignment is not a power of 2");
    ANTON_VERIFY((size / alignment) * alignment == size,
                 "size is not an integral multiple of alignment");
    return ANTON_CRT_ALIGNED_ALLOC(size, alignment);
  }

  void deallocate(void* memory)
  {
    ANTON_CRT_ALIGNED_FREE(memory);
  }

  void fill_memory(void* first, void* last, char8 value)
  {
    char8* f = reinterpret_cast<char8*>(first);
    char8* l = reinterpret_cast<char8*>(last);
    for(; f != l; ++f) {
      *f = value;
    }
  }

  void zero_memory(void* first, void* last)
  {
    char8* f = reinterpret_cast<char8*>(first);
    char8* l = reinterpret_cast<char8*>(last);
    for(; f != l; ++f) {
      *f = 0;
    }
  }
} // namespace anton
