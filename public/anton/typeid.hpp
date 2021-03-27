#include <anton/string_view.hpp>
#include <anton/types.hpp>

namespace anton {
    template<typename T>
    constexpr String_View type_name() {
        // TODO: Fix.
#if defined(__clang__) || defined(__GNUC__)
        constexpr String_View p = __PRETTY_FUNCTION__;
        // TODO: Adding u8 (u8" = ") makes the literal a char8_t string literal in C++17 (???)
        constexpr i64 type_offset = find_substring(p, u8" = ");
        return String_View{p.data() + type_offset + 3, p.data() + p.size_bytes() - 1};
#elif defined(_MSC_VER)
        constexpr String_View p = __FUNCSIG__;
        if(find_substring(p, "struct") != npos) {
            return String_View(p.data() + 58, p.size_bytes() - 58 - 7);
        } else if(find_substring(String_View(p.data() + 8, p.size_bytes() - 8), "class") != npos) {
            return String_View(p.data() + 57, p.size_bytes() - 57 - 7);
        } else {
            return String_View(p.data() + 51, p.size_bytes() - 51 - 7);
        }
#else
#    error "unknown compiler"
#endif
    }

    using Type_Identifier = u64;

    template<typename T>
    constexpr Type_Identifier type_identifier() {
        constexpr String_View name = type_name<T>();
        return anton::hash(name);
    }
} // namespace anton