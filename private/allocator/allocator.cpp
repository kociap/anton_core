#include <anton/allocator.hpp>

#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>

namespace anton {
  static Allocator default_allocator;

  Memory_Allocator* get_default_allocator()
  {
    return &default_allocator;
  }

  bool operator==(Memory_Allocator const& lhs, Memory_Allocator const& rhs)
  {
    return lhs.is_equal(rhs);
  }

  bool operator!=(Memory_Allocator const& lhs, Memory_Allocator const& rhs)
  {
    return !(lhs == rhs);
  }

  // Allocator

  void* Allocator::allocate(isize bytes, isize const alignment)
  {
    bytes = align_address(bytes, alignment);
    return anton::allocate(bytes, alignment);
  }

  void Allocator::deallocate(void* ptr, [[maybe_unused]] isize const bytes,
                             [[maybe_unused]] isize const alignment)
  {
    anton::deallocate(ptr);
  }

  bool Allocator::is_equal(Memory_Allocator const& other) const
  {
    // TODO: This should return true when other is an instance of Allocator
    //       (all instances of Allocator are equal).
    return this == &other;
  }

  void swap(Allocator&, Allocator&)
  {
    // Stateless allocator. No need to do anything.
  }

  bool operator==(Allocator const&, Allocator const&)
  {
    // All Allocators are stateless and may always be considered equal.
    return true;
  }

  bool operator!=(Allocator const&, Allocator const&)
  {
    // All Allocators are stateless and may always be considered equal.
    return false;
  }

  // Polymorphic Allocator

  Polymorphic_Allocator::Polymorphic_Allocator()
    : allocator(get_default_allocator())
  {
  }

  Polymorphic_Allocator::Polymorphic_Allocator(
    Polymorphic_Allocator const& other)
    : allocator(other.allocator)
  {
  }

  Polymorphic_Allocator::Polymorphic_Allocator(Polymorphic_Allocator&& other)
    : allocator(other.allocator)
  {
    other.allocator = get_default_allocator();
  }

  Polymorphic_Allocator::Polymorphic_Allocator(Memory_Allocator* new_allocator)
    : allocator(new_allocator)
  {
  }

  Polymorphic_Allocator&
  Polymorphic_Allocator::operator=(Polymorphic_Allocator&& other)
  {
    allocator = other.allocator;
    other.allocator = get_default_allocator();
    return *this;
  }

  void* Polymorphic_Allocator::allocate(isize size, isize alignment)
  {
    return allocator->allocate(size, alignment);
  }

  void Polymorphic_Allocator::deallocate(void* mem, isize size, isize alignment)
  {
    allocator->deallocate(mem, size, alignment);
  }

  Memory_Allocator* Polymorphic_Allocator::get_wrapped_allocator()
  {
    return allocator;
  }

  Memory_Allocator const* Polymorphic_Allocator::get_wrapped_allocator() const
  {
    return allocator;
  }

  void swap(Polymorphic_Allocator& allocator1,
            Polymorphic_Allocator& allocator2)
  {
    swap(allocator1.allocator, allocator2.allocator);
  }

  bool operator==(Polymorphic_Allocator const& lhs,
                  Polymorphic_Allocator const& rhs)
  {
    return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
  }

  bool operator!=(Polymorphic_Allocator const& lhs,
                  Polymorphic_Allocator const& rhs)
  {
    return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
  }
} // namespace anton
