#include <anton/ranges.hpp>

namespace anton {
  Range<Enumerate_Iterator> irange(isize begin, isize end)
  {
    return Range(Enumerate_Iterator(begin), Enumerate_Iterator(end));
  }
} // namespace anton
