#include <anton/allocator.hpp>

#include <anton/math/math.hpp>
#include <anton/memory/core.hpp>
#include <anton/swap.hpp>

namespace anton {
  [[nodiscard]] static void* align(void* const p, i64 const alignment)
  {
    return reinterpret_cast<void*>(
      align_address(reinterpret_cast<u64>(p), alignment));
  }

  [[nodiscard]] static i64 difference(void* const a, void* const b)
  {
    return static_cast<char*>(a) - static_cast<char*>(b);
  }

  [[nodiscard]] static void* advance(void* p, i64 const a)
  {
    return static_cast<char*>(p) + a;
  }

  Arena_Allocator::Arena_Allocator(i64 const default_block_size,
                                   i64 const default_block_alignment)
    : default_block_size(default_block_size),
      default_block_alignment(
        anton::math::max(default_block_alignment, (i64)alignof(Block)))
  {
  }

  Arena_Allocator::Arena_Allocator(Arena_Allocator&& allocator)
    : first(allocator.first), last(allocator.last),
      default_block_size(allocator.default_block_size),
      default_block_alignment(allocator.default_block_alignment)
  {
    // Move in default-constructed allocator to ensure allocator is usable.
    allocator = Arena_Allocator();
  }

  Arena_Allocator::~Arena_Allocator()
  {
    reset();
  }

  Arena_Allocator& Arena_Allocator::operator=(Arena_Allocator&& allocator)
  {
    anton::swap(first, allocator.first);
    anton::swap(last, allocator.last);
    anton::swap(default_block_size, allocator.default_block_size);
    anton::swap(default_block_alignment, allocator.default_block_alignment);
    return *this;
  }

  Arena_Allocator::Block* Arena_Allocator::allocate_block(i64 const size,
                                                          i64 const alignment)
  {
    i64 const allocation_alignment =
      anton::math::max(alignment, default_block_alignment);
    i64 const header = align_address(sizeof(Block), allocation_alignment);
    i64 const allocation_size =
      align_address(anton::math::max(size + header, default_block_size),
                    allocation_alignment);
    void* const memory = anton::allocate(allocation_size, allocation_alignment);
    Block* const block = reinterpret_cast<Block*>(memory);
    block->next = nullptr;
    block->free = advance(memory, sizeof(Block));
    block->end = advance(memory, allocation_size);
    return block;
  }

  void* Arena_Allocator::allocate(i64 const size, i64 const alignment)
  {
    if(!last) {
      Block* block = allocate_block(size, alignment);
      first = block;
      last = block;
      owned_memory_amount += difference(block->end, block);
    }

    void* const aligned = align(last->free, alignment);
    i64 const space = difference(last->end, aligned);
    if(space >= size) {
      last->free = advance(aligned, size);
      return aligned;
    } else {
      Block* const block = allocate_block(size, alignment);
      last->next = block;
      last = block;
      owned_memory_amount += difference(block->end, block);
      void* const aligned2 = align(block->free, alignment);
      block->free = advance(aligned2, size);
      return aligned2;
    }
  }

  void Arena_Allocator::deallocate(void*, i64, i64) {}

  bool Arena_Allocator::is_equal(Memory_Allocator const& allocator) const
  {
    return this == &allocator;
  }

  void Arena_Allocator::reset()
  {
    for(Block* block = first; block != nullptr;) {
      Block* const next = block->next;
      anton::deallocate(block);
      block = next;
    }

    first = nullptr;
    last = nullptr;
    owned_memory_amount = 0;
  }

  i64 Arena_Allocator::owned_memory() const
  {
    return owned_memory_amount;
  }

  void swap(Arena_Allocator& lhs, Arena_Allocator& rhs)
  {
    swap(lhs.first, rhs.first);
    swap(lhs.last, rhs.last);
    swap(lhs.default_block_alignment, rhs.default_block_alignment);
    swap(lhs.default_block_size, rhs.default_block_size);
    swap(lhs.owned_memory_amount, rhs.owned_memory_amount);
  }

  bool operator==(Arena_Allocator const& lhs, Arena_Allocator const& rhs)
  {
    return &lhs == &rhs;
  }

  bool operator!=(Arena_Allocator const& lhs, Arena_Allocator const& rhs)
  {
    return &lhs != &rhs;
  }
} // namespace anton
