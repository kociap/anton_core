#include <anton/memory.hpp>

#include <anton/assert.hpp>

namespace anton {
    void* allocate(i64 size, i64 alignment) {
        ANTON_VERIFY(alignment > 0 && !(alignment & (alignment - 1)), "alignment is not a power of 2");
        return _aligned_malloc(size, alignment);
    }

    void deallocate(void* memory) {
        _aligned_free(memory);
    }
} // namespace anton
