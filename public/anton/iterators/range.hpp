#pragma once

#include <anton/type_traits/utility.hpp>

namespace anton {
    template<typename T>
    struct Range {
    public:
        Range(T const& begin, T const& end): _begin(begin), _end(end) {}
        Range(T&& begin, T&& end): _begin(ANTON_MOV(begin)), _end(ANTON_MOV(end)) {}

        T begin() {
            return _begin;
        }

        T end() {
            return _end;
        }

    private:
        T _begin;
        T _end;
    };

    template<typename T>
    Range(T const&, T const&) -> Range<anton::remove_reference<T>>;
    template<typename T>
    Range(T&&, T&&) -> Range<anton::remove_reference<T>>;
} // namespace anton
