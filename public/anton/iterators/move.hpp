#pragma once

#include <anton/iterators/base.hpp>

namespace anton {
  template<typename Iterator>
  struct Move_Iterator {
  public:
    using value_type = typename Iterator_Traits<Iterator>::value_type;
    // using pointer =
    using reference = decltype(ANTON_MOV(*anton::declval<Iterator>()));
    using difference_type = typename Iterator_Traits<Iterator>::difference_type;
    using iterator_category =
      typename Iterator_Traits<Iterator>::iterator_category;
    using wrapped_iterator_type = Iterator;

    explicit Move_Iterator(wrapped_iterator_type const& iterator)
      : _iterator(iterator)
    {
    }
    explicit Move_Iterator(wrapped_iterator_type&& iterator)
      : _iterator(ANTON_MOV(iterator))
    {
    }

    wrapped_iterator_type const& base() const
    {
      return _iterator;
    }

    Move_Iterator& operator++()
    {
      ++_iterator;
      return *this;
    }

    Move_Iterator& operator--()
    {
      --_iterator;
      return *this;
    }

    Move_Iterator operator++(int)
    {
      auto copy = *this;
      ++_iterator;
      return copy;
    }

    Move_Iterator operator--(int)
    {
      auto copy = *this;
      --_iterator;
      return copy;
    }

    Move_Iterator& operator+=(difference_type n)
    {
      _iterator += n;
      return *this;
    }

    Move_Iterator& operator-=(difference_type n)
    {
      _iterator -= n;
      return *this;
    }

    [[nodiscard]] Move_Iterator operator+(difference_type n)
    {
      return Move_Iterator(_iterator + n);
    }

    [[nodiscard]] Move_Iterator operator-(difference_type n)
    {
      return Move_Iterator(_iterator - n);
    }

    [[nodiscard]] reference operator*() const
    {
      return ANTON_MOV(*_iterator);
    }

    [[nodiscard]] reference operator[](difference_type n) const
    {
      return *(*this + n);
    }

  private:
    wrapped_iterator_type _iterator;
  };

  template<typename T>
  Move_Iterator(T) -> Move_Iterator<T>;

  template<typename T>
  [[nodiscard]] constexpr Move_Iterator<T>
  operator+(typename Move_Iterator<T>::difference_type n,
            Move_Iterator<T> const& a)
  {
    return Move_Iterator<T>(a.base() + n);
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr auto
  operator-(Move_Iterator<T1> const& a,
            Move_Iterator<T2> const& b) -> decltype(a.base() - b.base())
  {
    return a.base() - b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator==(Move_Iterator<T1> const& a,
                                          Move_Iterator<T2> const& b)
  {
    return a.base() == b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator!=(Move_Iterator<T1> const& a,
                                          Move_Iterator<T2> const& b)
  {
    return a.base() != b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator<(Move_Iterator<T1> const& a,
                                         Move_Iterator<T2> const& b)
  {
    return a.base() < b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator>(Move_Iterator<T1> const& a,
                                         Move_Iterator<T2> const& b)
  {
    return a.base() > b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator<=(Move_Iterator<T1> const& a,
                                          Move_Iterator<T2> const& b)
  {
    return a.base() <= b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] constexpr bool operator>=(Move_Iterator<T1> const& a,
                                          Move_Iterator<T2> const& b)
  {
    return a.base() >= b.base();
  }
} // namespace anton
