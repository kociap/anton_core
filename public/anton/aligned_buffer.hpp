#pragma once

#include <anton/types.hpp>

namespace anton {
    template<usize size, usize alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton
