#pragma once

#include <anton/types.hpp>

namespace anton {
#ifndef ANTON_HAS_NO_BUILTIN_ADDRESSOF
  #define ANTON_ADDRESSOF(x) __builtin_addressof(x)
#else
  // decltype(x) will deduce the type of the expression unless it is
  // parenthesised in which case the type will be deudced to be a reference and
  // the compilation will fail.
  #define ANTON_ADDRESSOF(x)        \
    reinterpret_cast<decltype(x)*>( \
      &const_cast<char&>(reinterpret_cast<char const&>(x)))
#endif

#define ANTON_LAUNDER(p) __builtin_launder(p)

// Because we need ANTON_FWD here, we provide a duplicate definition of it in
// this header to avoid including <anton/utility.hpp> which includes traits
// header that is heavy.
#ifndef ANTON_FWD
  #define ANTON_FWD(x) (static_cast<decltype(x)&&>(x))
#endif

  // addressof
  template<typename T>
  [[nodiscard]] constexpr T* addressof(T& value)
  {
#ifndef ANTON_HAS_NO_BUILTIN_ADDRESSOF
    return __builtin_addressof(value);
#else
    // Note: We do not support volatile.
    return reinterpret_cast<T*>(
      &const_cast<char&>(reinterpret_cast<char const&>(value)));
#endif
  }

  // Issue 2598 "addressof works on temporaries". Deleted to prevent taking the
  // address of temporaries.
  template<typename T>
  T const* addressof(T const&&) = delete;

  template<typename T>
  [[nodiscard]] constexpr T* launder(T* ptr)
  {
    // TODO: Diagnostic for "N4727 21.6.4 [ptr.launder]/3: The program is
    //       ill-formed if T is a function type or void const."?
    return __builtin_launder(ptr);
  }

  // align_address
  // Round address up to alignment. If address is already aligned, does not
  // change address.
  //
  // Parameters:
  //   address - address to be aligned.
  // alignment - target alignment. Must be a power of 2.
  //
  // Returns:
  // Address aligned to alignment.
  //
  [[nodiscard]] constexpr u64 align_address(u64 const address,
                                            u64 const alignment)
  {
    return (address + (alignment - 1)) & ~(alignment - 1);
  }

  // allocate
  // Allocates a block of memory aligned to alignment.
  //
  // Parameters:
  //     size  - the number of bytes to allocate. Must be an integral multiple
  //             of alignment.
  // alignment - alignment of the memory block. Must be a power of 2.
  //
  // Returns:
  // A newly allocated block of memory aligned to alignment or nullptr if
  // allocation failed.
  //
  [[nodiscard]] void* allocate(i64 size, i64 alignment);

  // deallocate
  // Deallocates a block of memory allocated by allocate.
  //
  // Parameters:
  // memory - pointer to the memory block. Might be nullptr in which case the
  //          function does nothing.
  //
  void deallocate(void* memory);

  // fill_memory
  // Fill the memory range [first, last[ with value.
  //
  // Parameters:
  // first - The beginning of the memory range to fill. May be nullptr;
  //  last - The end of the memory range to fill. If first is nullptr, must also
  //         be nullptr.
  // value - The value to fill the memory range with.
  //
  void fill_memory(void* first, void* last, char8 value);

  // zero_memory
  // Fill the memory range [first, last[ with zeros.
  //
  // Parameters:
  // first - The beginning of the memory range to fill. May be nullptr;
  //  last - The end of the memory range to fill. If first is nullptr, must also
  //         be nullptr.
  //
  void zero_memory(void* first, void* last);

  // construct
  // Construct an object of type T at pointer with args forwarded to the
  // constructor.
  //
  // Parameters:
  // pointer - memory location where to construct the object.
  // args... - arguments to forward to the constructor.
  //
  template<typename T, typename... Args>
  auto construct(T* pointer, Args&&... args) -> T*
  {
    // We use the compiler intrinsic to remove the depencendy on type_traits in
    // order to make this header as lightweight as possible.
    if constexpr(__is_constructible(T, decltype(ANTON_FWD(args))...)) {
      ::new(reinterpret_cast<void*>(pointer)) T(ANTON_FWD(args)...);
    } else {
      ::new(reinterpret_cast<void*>(pointer)) T{ANTON_FWD(args)...};
    }
    return pointer;
  }

  // destruct
  // Destruct an object of type T at pointer.
  //
  // Parameters:
  // pointer - pointer to the object to destruct.
  //
  template<typename T>
  void destruct(T* pointer)
  {
    pointer->~T();
  }

  // new_obj
  // Allocate memory and construct a single new object of type T.
  //
  // Parameters:
  // args... - arguments to forward to the constructor.
  //
  template<typename T, typename... Args>
  [[nodiscard]] T* new_obj(Args&&... args)
  {
    T* const p = reinterpret_cast<T*>(allocate(sizeof(T), alignof(T)));
    construct(p, ANTON_FWD(args)...);
    return p;
  }

  // delete_obj
  // Destruct a single object of type T allocated by new_obj and deallocate
  // memory.
  //
  // Parameters:
  // pointer - pointer to the object to delete.
  //
  template<typename T>
  void delete_obj(T* pointer)
  {
    pointer->~T();
    deallocate(pointer);
  }
} // namespace anton
