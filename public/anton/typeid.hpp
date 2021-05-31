#pragma once

#include <anton/string_view.hpp>
#include <anton/types.hpp>

namespace anton {
    // type_name
    // Obtain the name of the type passed as the template parameter.
    //
    // Returns:
    // Name of the type passed as the template parameter.
    // If the type is an alias, returns the name of the type
    // that is being aliased.
    //
    template<typename T>
    constexpr String_View type_name() {
#if ANTON_COMPILER_CLANG
        // __PRETTY_FUNCTION__ will be defined as:
        //     String_View type_name() [T = name]
        // Aliases will be expanded. We want to extract the name from the string.
        constexpr String_View fn = __PRETTY_FUNCTION__;
        constexpr i64 begin_offset = 29;
        constexpr i64 end_offset = 1;
        return String_View{fn.bytes_begin() + begin_offset, fn.bytes_end() - end_offset};
#elif ANTON_COMPILER_MSVC
        // __FUNCSIG__ will be defined as:
        // - for structs:
        //     struct String_View __cdecl type_name<struct name>(void)
        // - for classes:
        //     struct String_View __cdecl type_name<class name>(void)
        // - for enums:
        //     struct String_View __cdecl type_name<enum name>(void)
        // - for other types:
        //     struct String_View __cdecl type_name<name>(void)
        // Aliases will be expanded. We want to extract the name from the string.
        constexpr String_View fn = __FUNCSIG__;
        constexpr i64 begin_offset = 37;
        constexpr i64 end_offset = 7;
        constexpr String_View qualified_type{fn.bytes_begin() + begin_offset, fn.bytes_end() - end_offset};
        // Remove the qualification
        if constexpr(begins_with(qualified_type, u8"struct "_sv)) {
            return String_View{qualified_type.bytes_begin() + 7, qualified_type.bytes_end()};
        } else if constexpr(begins_with(qualified_type, u8"class "_sv)) {
            return String_View{qualified_type.bytes_begin() + 6, qualified_type.bytes_end()};
        } else if constexpr(begins_with(qualified_type, u8"enum "_sv)) {
            return String_View{qualified_type.bytes_begin() + 5, qualified_type.bytes_end()};
        } else {
            return qualified_type;
        }
#else
#    error "unknown compiler"
#endif
    }

    using Type_Identifier = u64;

    // type_identifier
    // Obtain a unique identifier of the type passed as the template parameter.
    //
    // Returns:
    // Identifier of the type passed as the template parameter.
    // If the type is an alias, returns the identifier of the type
    // that is being aliased.
    //
    template<typename T>
    constexpr Type_Identifier type_identifier() {
        constexpr String_View name = type_name<T>();
        return anton::hash(name);
    }
} // namespace anton
