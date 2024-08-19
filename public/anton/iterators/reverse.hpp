#pragma once

#include <anton/iterators/base.hpp>

namespace anton {
  template<typename Iterator>
  struct Reverse_Iterator {
  public:
    using value_type = typename Iterator_Traits<Iterator>::value_type;
    using pointer = typename Iterator_Traits<Iterator>::pointer;
    using reference = typename Iterator_Traits<Iterator>::reference;
    using difference_type = typename Iterator_Traits<Iterator>::difference_type;
    using iterator_category =
      typename Iterator_Traits<Iterator>::iterator_category;
    using wrapped_iterator_type = Iterator;

    explicit Reverse_Iterator(wrapped_iterator_type const& iter)
      : _iterator(iter)
    {
    }
    explicit Reverse_Iterator(wrapped_iterator_type&& iter)
      : _iterator(ANTON_MOV(iter))
    {
    }

    wrapped_iterator_type const& base() const
    {
      return _iterator;
    }

    Reverse_Iterator& operator++()
    {
      --_iterator;
      return *this;
    }

    Reverse_Iterator& operator--()
    {
      ++_iterator;
      return *this;
    }

    Reverse_Iterator operator++(int)
    {
      auto copy = *this;
      --_iterator;
      return copy;
    }

    Reverse_Iterator operator--(int)
    {
      auto copy = *this;
      ++_iterator;
      return copy;
    }

    Reverse_Iterator& operator+=(difference_type n)
    {
      _iterator -= n;
      return *this;
    }

    Reverse_Iterator& operator-=(difference_type n)
    {
      _iterator += n;
      return *this;
    }

    [[nodiscard]] Reverse_Iterator operator+(difference_type n)
    {
      return Reverse_Iterator(_iterator - n);
    }

    [[nodiscard]] Reverse_Iterator operator-(difference_type n)
    {
      return Reverse_Iterator(_iterator + n);
    }

    [[nodiscard]] reference operator*() const
    {
      return *(_iterator - 1);
    }

    [[nodiscard]] wrapped_iterator_type operator->() const
    {
      return _iterator - 1;
    }

    [[nodiscard]] reference operator[](difference_type n) const
    {
      return *(*this + n);
    }

  private:
    wrapped_iterator_type _iterator;
  };

  template<typename T>
  Reverse_Iterator(T) -> Reverse_Iterator<T>;

  template<typename T>
  [[nodiscard]] Reverse_Iterator<T>
  operator+(typename Reverse_Iterator<T>::difference_type n,
            Reverse_Iterator<T> const& a)
  {
    return Reverse_Iterator<T>(a.base() - n);
  }

  template<typename T1, typename T2>
  [[nodiscard]] auto
  operator-(Reverse_Iterator<T1> const& a,
            Reverse_Iterator<T2> const& b) -> decltype(b.base() - a.base())
  {
    return b.base() - a.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator==(Reverse_Iterator<T1> const& a,
                                Reverse_Iterator<T2> const& b)
  {
    return a.base() == b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator!=(Reverse_Iterator<T1> const& a,
                                Reverse_Iterator<T2> const& b)
  {
    return a.base() != b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator<(Reverse_Iterator<T1> const& a,
                               Reverse_Iterator<T2> const& b)
  {
    return a.base() > b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator>(Reverse_Iterator<T1> const& a,
                               Reverse_Iterator<T2> const& b)
  {
    return a.base() < b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator<=(Reverse_Iterator<T1> const& a,
                                Reverse_Iterator<T2> const& b)
  {
    return a.base() >= b.base();
  }

  template<typename T1, typename T2>
  [[nodiscard]] bool operator>=(Reverse_Iterator<T1> const& a,
                                Reverse_Iterator<T2> const& b)
  {
    return a.base() <= b.base();
  }
} // namespace anton
