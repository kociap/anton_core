#pragma once

#include <anton/iterators.hpp>
#include <anton/swap.hpp>
#include <anton/type_traits.hpp>
#include <anton/types.hpp>
#include <anton/utility.hpp>

namespace anton {
  template<typename T>
  struct Slice;

  template<typename T>
  struct Is_Slice: False_Type {};

  template<typename T>
  struct Is_Slice<Slice<T>>: True_Type {};

  template<typename T>
  struct Is_Slice<Slice<T> const>: True_Type {};

  template<typename T>
  constexpr bool is_slice = Is_Slice<T>::value;

  template<typename T>
  struct Slice {
  public:
    using value_type = T;
    using size_type = i64;
    using difference_type = isize;
    using iterator = T*;
    using const_iterator = T const*;

    constexpr Slice(): _data(nullptr), _size(0) {}
    template<typename Integral, enable_if<is_integral<Integral>, i64> = 0>
    constexpr Slice(T* const first, Integral const length)
      : _data(first), _size(length)
    {
    }
    constexpr Slice(T* const first, T* const last)
      : _data(first), _size(last - first)
    {
    }
    template<i64 N>
    constexpr Slice(T (&array)[N]): _data(array), _size(N)
    {
    }
    template<typename Container>
    constexpr Slice(
      Container& c,
      enable_if<!is_slice<Container> &&
                  is_convertible<remove_pointer<decltype(anton::data(c))> (*)[],
                                 value_type (*)[]>,
                void*> = nullptr)
      : _data(anton::data(c)), _size(anton::size(c))
    {
    }
    template<typename U>
    constexpr Slice(
      Slice<U> const& other,
      enable_if<is_convertible<U (*)[], value_type (*)[]>, void*> = nullptr)
      : _data(other.data()), _size(other.size())
    {
    }
    constexpr Slice(Slice const& other): _data(other._data), _size(other._size)
    {
    }

    void operator=(Slice const& other)
    {
      _data = other._data;
      _size = other._size;
    }

    T& operator[](size_type i) const
    {
      return *(_data + i);
    }

    iterator begin() const
    {
      return _data;
    }

    const_iterator cbegin() const
    {
      return _data;
    }

    iterator end() const
    {
      return _data + _size;
    }

    const_iterator cend() const
    {
      return _data + _size;
    }

    size_type size() const
    {
      return _size;
    }

    T* data() const
    {
      return _data;
    }

    Slice subslice(size_type first, size_type last) const
    {
      return {_data + first, _data + last};
    }

    friend void swap(Slice& lhs, Slice& rhs)
    {
      swap(lhs._data, rhs._data);
      swap(lhs._size, rhs._size);
    }

  private:
    T* _data;
    size_type _size;
  };

  template<typename T, i64 N>
  Slice(T (&)[N]) -> Slice<T>;
} // namespace anton
