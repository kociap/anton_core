#include <anton/assert.hpp>

#include <stdexcept>

namespace anton {
    void anton_assert(char8 const* message, char8 const* file, u64 line) {
        // TODO: We throw a crashception, because I'm too lazy rn to port anton assert correctly.
        throw std::runtime_error(message);
    }
}