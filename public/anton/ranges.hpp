#pragma once

#include <anton/iterators/enumerate.hpp>
#include <anton/iterators/zip.hpp>

namespace anton {
  template<typename T>
  struct Range {
  public:
    Range(T const& begin, T const& end): _begin(begin), _end(end) {}
    Range(T&& begin, T&& end): _begin(ANTON_MOV(begin)), _end(ANTON_MOV(end)) {}

    [[nodiscard]] T begin()
    {
      return _begin;
    }

    [[nodiscard]] T end()
    {
      return _end;
    }

  private:
    T _begin;
    T _end;
  };

  template<typename T>
  Range(T const&, T const&) -> Range<remove_reference<T>>;
  template<typename T>
  Range(T&&, T&&) -> Range<remove_reference<T>>;

  Range<Enumerate_Iterator> irange(isize begin, isize end);

  template<typename... Rangelike>
  auto zip(Rangelike&&... rangelike)
  {
    static_assert(sizeof...(rangelike) > 1,
                  "zip requires at least 2 range-like objects");
    return Range(Zip_Iterator(rangelike.begin()...),
                 Zip_Iterator(rangelike.end()...));
  }

  template<typename Rangelike>
  auto enumerate(Rangelike&& rangelike)
  {
    return zip(
      Range(Enumerate_Iterator(0), Enumerate_Iterator(limits::maximum_i64)),
      rangelike);
  }
} // namespace anton
