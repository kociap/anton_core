#pragma once

#include <anton/allocator.hpp>
#include <anton/functors.hpp>
#include <anton/iterators.hpp>
#include <anton/string7_view.hpp>
#include <anton/tags.hpp>

namespace anton {
    // String7
    // UTF-8 encoded string containing only the ASCII subset.
    //
    struct String7 {
    public:
        using value_type = char8;
        using size_type = i64;
        using difference_type = isize;
        using allocator_type = Polymorphic_Allocator;
        using iterator = char8*;
        using const_iterator = char8*;

    public:
        String7();
        explicit String7(allocator_type const& allocator);
        // Reserve space to fit a string of length n and null-terminator.
        explicit String7(Reserve_Tag, size_type n);
        // Reserve space to fit a string of length n and null-terminator.
        explicit String7(Reserve_Tag, size_type n, allocator_type const& allocator);
        // Constructs String7 from null-terminated UTF-8 string
        explicit String7(value_type const* string);
        // Constructs String7 from null-terminated UTF-8 string
        explicit String7(value_type const* string, allocator_type const& allocator);
        // Constructs String7 from a string of size n
        explicit String7(value_type const* string, size_type n);
        // Constructs String7 from a string of size n
        explicit String7(value_type const* string, size_type n, allocator_type const& allocator);
        // Constructs String7 from the range [first, last[
        explicit String7(value_type const* first, value_type const* last);
        // Constructs String7 from the range [first, last[
        explicit String7(value_type const* first, value_type const* last, allocator_type const& allocator);
        explicit String7(String7_View string);
        explicit String7(String7_View string, allocator_type const& allocator);
        // Copies the allocator
        String7(String7 const& other);
        String7(String7 const& other, allocator_type const& allocator);
        // Moves the allocator
        String7(String7&& other);
        String7(String7&& other, allocator_type const& allocator);
        ~String7();

        // Does not copy the allocator
        String7& operator=(String7 const& other);
        // Does not move the allocator
        String7& operator=(String7&& other);
        String7& operator=(String7_View string);
        String7& operator=(value_type const* cstr);

    public:
        // Implicit conversion operator
        [[nodiscard]] operator String7_View() const;

    public:
        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        [[nodiscard]] value_type* data();
        [[nodiscard]] value_type const* data() const;
        [[nodiscard]] value_type const* c_str() const;

        [[nodiscard]] value_type operator[](size_type index) const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator cbegin() const;

        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cend() const;

        // Capacity of the string in bytes.
        [[nodiscard]] size_type capacity() const;
        // Size of the string in bytes.
        [[nodiscard]] size_type size() const;

        // ensure_capacity
        // Allocates at least requested_capacity + 1 (for null-terminator) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void ensure_capacity(size_type requested_capacity);

        // ensure_capacity_exact
        // Allocates exactly requested_capacity (null-terminator is not accounted for) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void ensure_capacity_exact(size_type requested_capacity);

        // force_size
        // Changes the size of the string to n. Useful in situations when the user
        // writes to the string via external means.
        //
        void force_size(size_type n);

        void clear();
        void append(char8 c);
        void append(String7_View string);
        // template <typename Input_Iterator>
        // iterator insert(const_iterator pos, Input_Iterator first, Input_Iterator last);

        friend void swap(String7& lhs, String7& rhs);

    private:
        allocator_type _allocator;
        value_type* _data = nullptr;
        size_type _capacity = 0;
        size_type _size = 0;
    };

    inline namespace literals {
        [[nodiscard]] String7 operator""_s7(char8 const* literal, u64 size);
    }

    [[nodiscard]] bool operator==(String7 const& lhs, String7 const& rhs);
    [[nodiscard]] bool operator!=(String7 const& lhs, String7 const& rhs);

    String7& operator+=(String7& lhs, char8 rhs);
    String7& operator+=(String7& lhs, String7_View rhs);

    [[nodiscard]] String7 operator+(String7 const& lhs, String7 const& rhs);
    [[nodiscard]] String7 operator+(String7_View lhs, String7 const& rhs);
    [[nodiscard]] String7 operator+(String7 const& lhs, String7_View rhs);
    [[nodiscard]] String7 operator+(char8 const* lhs, String7 const& rhs);
    [[nodiscard]] String7 operator+(String7 const& lhs, char8 const* rhs);

    [[nodiscard]] String7 to_string7(i32 v);
    [[nodiscard]] String7 to_string7(u32 v);
    [[nodiscard]] String7 to_string7(i64 v);
    [[nodiscard]] String7 to_string7(u64 v);
    [[nodiscard]] String7 to_string7(f32 v);
    [[nodiscard]] String7 to_string7(f64 v);
    [[nodiscard]] String7 to_string7(void const* v);

    // TODO: Implement in terms of String_View.
    [[nodiscard]] f32 str_to_f32(String7 const& string);

    template<>
    struct Default_Hash<String7> {
        using transparent = void;

        [[nodiscard]] constexpr u64 operator()(String7_View const v) const {
            return anton::hash(v);
        }
    };

    template<>
    struct Equal_Compare<String7> {
        using transparent = void;

        [[nodiscard]] constexpr bool operator()(String7_View const lhs, String7_View const rhs) const {
            return lhs == rhs;
        }
    };
} // namespace anton
