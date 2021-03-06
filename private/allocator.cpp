#include <anton/allocator.hpp>

#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>

namespace anton {
    static Allocator default_allocator;

    Memory_Allocator* get_default_allocator() {
        return &default_allocator;
    }

    bool operator==(Memory_Allocator const& lhs, Memory_Allocator const& rhs) {
        return lhs.is_equal(rhs);
    }

    bool operator!=(Memory_Allocator const& lhs, Memory_Allocator const& rhs) {
        return !(lhs == rhs);
    }

    // Allocator

    void* Allocator::allocate(isize const bytes, isize const alignment) {
        return anton::allocate(bytes, alignment);
    }

    void Allocator::deallocate(void* ptr, [[maybe_unused]] isize const bytes, [[maybe_unused]] isize const alignment) {
        anton::deallocate(ptr);
    }

    bool Allocator::is_equal(Memory_Allocator const& other) const {
        return this == &other;
    }

    void swap(Allocator&, Allocator&) {
        // Stateless allocator. No need to do anything.
    }

    bool operator==(Allocator const&, Allocator const&) {
        return true; // All Allocators are stateless and may always be considered equal.
    }

    bool operator!=(Allocator const&, Allocator const&) {
        return false; // All Allocators are stateless and may always be considered equal.
    }

    // Buffer Allocator

    [[maybe_unused]] static char* adjust_to_alignment(char* address, usize alignment) {
        usize misalignment = (reinterpret_cast<usize>(address) & (alignment - 1));
        address += (misalignment != 0 ? alignment - misalignment : 0);
        return address;
    }

    // Buffer_Allocator::Buffer_Allocator(char* b, char* e): begin(adjust_to_alignment(b, alignof(Block_Data))), end(e) {
    //     Block_Data* block_data = reinterpret_cast<Block_Data*>(begin);
    //     *block_data = Block_Data{};
    // }

    // void* Buffer_Allocator::allocate(isize size, isize alignment) {
    //     auto aligned_block_size = [](Block_Data* block_begin, Block_Data* block_end, usize alignment) {
    //         char* begin = adjust_to_alignment(reinterpret_cast<char*>(block_begin + 1), alignment);
    //         char* end = reinterpret_cast<char*>(block_end);
    //         return end - begin;
    //     };
    // }

    // void Buffer_Allocator::deallocate(void*, isize size, isize alignment);
    // bool Buffer_Allocator::is_equal(Memory_Allocator const& other) const;

    // Polymorphic Allocator

    Polymorphic_Allocator::Polymorphic_Allocator(): allocator(get_default_allocator()) {}

    Polymorphic_Allocator::Polymorphic_Allocator(Polymorphic_Allocator const& other): allocator(other.allocator) {}

    Polymorphic_Allocator::Polymorphic_Allocator(Polymorphic_Allocator&& other): allocator(other.allocator) {
        other.allocator = get_default_allocator();
    }

    Polymorphic_Allocator::Polymorphic_Allocator(Memory_Allocator* new_allocator): allocator(new_allocator) {}

    Polymorphic_Allocator& Polymorphic_Allocator::operator=(Polymorphic_Allocator&& other) {
        allocator = other.allocator;
        other.allocator = get_default_allocator();
        return *this;
    }

    void* Polymorphic_Allocator::allocate(isize size, isize alignment) {
        return allocator->allocate(size, alignment);
    }

    void Polymorphic_Allocator::deallocate(void* mem, isize size, isize alignment) {
        allocator->deallocate(mem, size, alignment);
    }

    Memory_Allocator* Polymorphic_Allocator::get_wrapped_allocator() {
        return allocator;
    }

    Memory_Allocator const* Polymorphic_Allocator::get_wrapped_allocator() const {
        return allocator;
    }

    void swap(Polymorphic_Allocator& allocator1, Polymorphic_Allocator& allocator2) {
        swap(allocator1.allocator, allocator2.allocator);
    }

    bool operator==(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }

    bool operator!=(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }
} // namespace anton
