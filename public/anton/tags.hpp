#pragma once

namespace anton {
    // Reserve_Tag
    // Allows us to create constructors that allocate certain capacity, but do not initialize/resize
    struct Reserve_Tag {
        explicit constexpr Reserve_Tag() = default; // Explicit constructor so that it may not be constructed via {}
    };
    constexpr Reserve_Tag reserve;

    // Variadic_Construct_Tag
    // Allows us to create variadic constructors
    struct Variadic_Construct_Tag {
        explicit constexpr Variadic_Construct_Tag() = default; // Explicit constructors so that it may not be constructed via {}
    };
    constexpr Variadic_Construct_Tag variadic_construct;

    // Range_Construct_Tag
    // For templated constructor overloads that take a pair of iterators
    struct Range_Construct_Tag {
        explicit constexpr Range_Construct_Tag() = default; // Explicit constructors so that it may not be constructed via {}
    };
    constexpr Range_Construct_Tag range_construct;
} // namespace anton
