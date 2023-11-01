#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/diagnostic_macros.hpp>
#include <anton/types.hpp>

namespace anton {
    // Memory_Allocator
    // An abstract class that provides an interface for all allocators that are supposed
    // to be used with Polymorphic_Allocator in polymorphic containers.
    //
    struct Memory_Allocator {
    public:
        virtual ~Memory_Allocator() = default;

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR virtual void* allocate(isize size, isize alignment) = 0;
        virtual void deallocate(void*, isize size, isize alignment) = 0;
        [[nodiscard]] virtual bool is_equal(Memory_Allocator const&) const = 0;
    };

    [[nodiscard]] bool operator==(Memory_Allocator const& lhs, Memory_Allocator const& rhs);
    [[nodiscard]] bool operator!=(Memory_Allocator const& lhs, Memory_Allocator const& rhs);

    // get_default_allocator
    //
    [[nodiscard]] Memory_Allocator* get_default_allocator();

    // Allocator
    // A generic allocator that provides malloc like functionality to allocate properly aligned memory.
    // This allocator class is not templated on any type and inherits from Memory_Allocator
    // to allow it to be used in polymorphic containers.
    //
    struct Allocator: public Memory_Allocator {
    public:
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment) override;
        void deallocate(void*, isize size, isize alignment) override;
        [[nodiscard]] bool is_equal(Memory_Allocator const& other) const override;
    };

    void swap(Allocator& v1, Allocator& v2);

    [[nodiscard]] bool operator==(Allocator const& lhs, Allocator const& rhs);
    [[nodiscard]] bool operator!=(Allocator const& lhs, Allocator const& rhs);

    // Arena_Allocator
    //
    struct Arena_Allocator: public Allocator {
        Arena_Allocator(i64 default_block_size = 65536, i64 default_block_alignment = 8);
        Arena_Allocator(Arena_Allocator const& allocator) = delete;
        Arena_Allocator(Arena_Allocator&& allocator);
        ~Arena_Allocator() override;
        Arena_Allocator& operator=(Arena_Allocator const& allocator) = delete;
        Arena_Allocator& operator=(Arena_Allocator&& allocator);

        // allocate
        //
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR virtual void* allocate(i64 size, i64 alignment) override;

        // deallocate
        // Does nothing.
        //
        virtual void deallocate(void* memory, i64 size, i64 alignment) override;

        // is_equal
        // Compares two allocators. Two arena allocators are equal if and only if they are the same
        // object.
        //
        // Returns:
        // true if allocator is the same object as *this.
        //
        [[nodiscard]] virtual bool is_equal(Memory_Allocator const& allocator) const override;

        // reset
        // Frees all memory owned by the allocator without calling destructors and restores the
        // allocator to the default state.
        //
        void reset();

        // owned_memory
        // Obtains the total amount of memory owned by the allocator.
        //
        // Returns:
        // The amount of memory owned by the allocator in bytes.
        //
        [[nodiscard]] i64 owned_memory() const;

        friend void swap(Arena_Allocator& lhs, Arena_Allocator& rhs);

    private:
        struct Block {
            Block* next = nullptr;
            // Pointer to the first free location in the block.
            void* free = nullptr;
            // Pointer to the end of the block.
            void* end = nullptr;
        };

        Block* first = nullptr;
        Block* last = nullptr;
        i64 default_block_size;
        i64 default_block_alignment;
        i64 owned_memory_amount = 0;

        Block* allocate_block(i64 size, i64 alignment);
    };

    [[nodiscard]] bool operator==(Arena_Allocator const& lhs, Arena_Allocator const& rhs);
    [[nodiscard]] bool operator!=(Arena_Allocator const& lhs, Arena_Allocator const& rhs);

    // Polymorphic_Allocator
    // A wrapper around Memory_Allocator to allow any custom allocator to be used with any
    // container without baking the allocator type into container type.
    //
    struct Polymorphic_Allocator {
    public:
        Polymorphic_Allocator();
        Polymorphic_Allocator(Polymorphic_Allocator const&);
        Polymorphic_Allocator(Polymorphic_Allocator&&);
        Polymorphic_Allocator(Memory_Allocator*);
        ~Polymorphic_Allocator() = default;

        Polymorphic_Allocator& operator=(Polymorphic_Allocator const&) = delete;
        Polymorphic_Allocator& operator=(Polymorphic_Allocator&&);

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment);
        void deallocate(void*, isize size, isize alignment);

        Memory_Allocator* get_wrapped_allocator();
        Memory_Allocator const* get_wrapped_allocator() const;

        friend void swap(Polymorphic_Allocator&, Polymorphic_Allocator&);

    private:
        Memory_Allocator* allocator;
    };

    [[nodiscard]] bool operator==(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
    [[nodiscard]] bool operator!=(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
} // namespace anton
