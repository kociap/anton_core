#pragma once

// Since volatile has been deprecated in C++20, this implementation provides no traits for removing, adding or identifying volatile.

// is_swappable are defined in swap.hpp
// is_iterator_wrapper is defined in iterators.hpp
// is_slice is defined in slice.hpp

#include <anton/type_traits/base.hpp>
#include <anton/type_traits/function.hpp>
#include <anton/type_traits/properties.hpp>
#include <anton/type_traits/transformations.hpp>
#include <anton/type_traits/utility.hpp>
