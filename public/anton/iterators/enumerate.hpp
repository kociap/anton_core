#pragma once

#include <anton/iterators/base.hpp>

namespace anton {
    struct Enumerate_Iterator {
    public:
        using value_type = isize;
        using reference = value_type;
        using difference_type = isize;
        using iterator_category = Input_Iterator_Tag;

    private:
        value_type value;

    public:
        Enumerate_Iterator(value_type value): value(value) {}

        [[nodiscard]] reference operator*() const {
            return value;
        }

        Enumerate_Iterator& operator++() {
            value += 1;
            return *this;
        }

        Enumerate_Iterator operator++(int) {
            Enumerate_Iterator copy = *this;
            value += 1;
            return copy;
        }

        [[nodiscard]] bool operator==(Enumerate_Iterator& other) const {
            return value == other.value;
        }

        [[nodiscard]] bool operator!=(Enumerate_Iterator& other) const {
            return value != other.value;
        }
    };
} // namespace anton
