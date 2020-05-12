#pragma once

// Since volatile has been deprecated in C++20, this implementation provides no traits for removing, adding or identifying volatile.

// is_swappable, is_noexcept_swappable are defined in utility.hpp
// is_iterator_wrapper is defined in iterators.hpp
// is_slice is defined in slice.hpp

#include <anton/detail/traits_base.hpp>
#include <anton/detail/traits_function.hpp>
#include <anton/detail/traits_properties.hpp>
#include <anton/detail/traits_transformations.hpp>
#include <anton/detail/traits_utility.hpp>
