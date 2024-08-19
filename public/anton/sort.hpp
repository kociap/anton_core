#pragma once

#include <anton/array.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>
#include <anton/type_traits/utility.hpp>

namespace anton {
  template<typename Forward_Iterator, typename Predicate>
  Forward_Iterator is_sorted(Forward_Iterator first, Forward_Iterator last,
                             Predicate predicate)
  {
    if(first != last) {
      Forward_Iterator i = first;
      while(++i != last) {
        if(predicate(*i, *first)) {
          return i;
        } else {
          first = i;
        }
      }
    }
    return first;
  }

  // bubble_sort
  // Stable in-place sort.
  //
  // Parameters:
  // first, last - the range to sort.
  //   predicate - comparison function. Returns true when the first argument is
  //               less than (i.e. ordered before) the second argument. The
  //               function must not modify the objects passed to it.
  //
  template<typename Forward_Iterator, typename Predicate>
  void bubble_sort(Forward_Iterator first, Forward_Iterator last,
                   Predicate predicate)
  {
    for(; first != last;) {
      Forward_Iterator i = first;
      Forward_Iterator next = first;
      ++next;
      bool swapped = false;
      for(; next != last; ++next, ++i) {
        if(predicate(*next, *i)) {
          swap(*i, *next);
          swapped = true;
        }
      }
      last = i;
      if(!swapped) {
        break;
      }
    }
  }

  template<typename Forward_Iterator>
  void bubble_sort(Forward_Iterator first, Forward_Iterator last)
  {
    using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
    bubble_sort(first, last, [](value_type const& lhs, value_type const& rhs) {
      return lhs < rhs;
    });
  }

  template<typename Random_Access_Iterator, typename Predicate>
  void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last,
                      Predicate predicate)
  {
    if(first == last) {
      return;
    }

    for(Random_Access_Iterator i = first + 1; i != last; ++i) {
      for(Random_Access_Iterator j = i; j != first;) {
        Random_Access_Iterator prev = j;
        --prev;
        if(!predicate(*prev, *j)) {
          swap(*j, *prev);
          j = prev;
        } else {
          break;
        }
      }
    }
  }

  template<typename Random_Access_Iterator>
  void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last)
  {
    using value_type =
      typename Iterator_Traits<Random_Access_Iterator>::value_type;
    insertion_sort(
      first, last,
      [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
  }

  template<typename Random_Access_Iterator, typename Predicate>
  void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last,
                  Predicate predicate)
  {
    auto const size = last - first;
    if(size < 2) {
      return;
    }

    if(size < 6) {
      bubble_sort(first, last, predicate);
      return;
    }

    Random_Access_Iterator pivot = first + size / 2;
    Random_Access_Iterator i = first;
    Random_Access_Iterator j = last - 1;
    while(true) {
      while(predicate(*i, *pivot)) {
        ++i;
      }

      while(predicate(*pivot, *j)) {
        --j;
      }

      if(i < j) {
        swap(*i, *j);
        if(i == pivot) {
          pivot = j;
        } else if(j == pivot) {
          pivot = i;
        }
        ++i;
        --j;
      } else {
        break;
      }
    }

    quick_sort(first, j + 1, predicate);
    quick_sort(j + 1, last, predicate);
  }

  template<typename Random_Access_Iterator>
  void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last)
  {
    using value_type =
      typename Iterator_Traits<Random_Access_Iterator>::value_type;
    quick_sort(first, last, [](value_type const& lhs, value_type const& rhs) {
      return lhs < rhs;
    });
  }

  // merge_sort
  // Stable non-in-place sort. Allocates additional memory proportional to
  // last - first.
  //
  // Parameters:
  // first, last - the range to sort.
  // predicate   - comparison function. Returns true when the first argument is
  //               less than (i.e. ordered before) the second argument. The
  //               function must not modify the objects passed to it.
  //
  template<typename Random_Access_Iterator, typename Predicate>
  void merge_sort(Random_Access_Iterator first, Random_Access_Iterator last,
                  Predicate p)
  {
    i64 const length = last - first;
    // Sort 4-long runs using bubble sort
    for(i64 i = 0; i < length; i += 4) {
      Random_Access_Iterator b = first + i;
      Random_Access_Iterator e = first + math::min(i + 4, length);
      bubble_sort(b, e, p);
    }

    if(length <= 4) {
      return;
    }

    using value_type =
      typename Iterator_Traits<Random_Access_Iterator>::value_type;
    Array<value_type> storage{reserve, length};
    storage.force_size(length);
    // TODO: Is there a way to avoid the copy?
    uninitialized_copy(first, last, storage.begin());
    auto const storage_begin = storage.begin();
    auto const storage_end = storage.end();
    // Direction of elements copying. If true, copy from input range to storage.
    bool copy_a_to_b = true;
    // Bottom-top merge sort
    for(i64 width = 4; width < length; width *= 2) {
      if(copy_a_to_b) {
        i64 i = 0;
        for(; i + width < length; i += 2 * width) {
          Random_Access_Iterator left = first + i;
          Random_Access_Iterator right = first + (i + width);
          Random_Access_Iterator end_l = right;
          Random_Access_Iterator end_r =
            first + math::min(i + 2 * width, length);
          auto out = storage_begin + i;
          for(; left != end_l && right != end_r; ++out) {
            bool const greater = p(*right, *left);
            if(!greater) {
              *out = ANTON_MOV(*left);
              ++left;
            } else {
              *out = ANTON_MOV(*right);
              ++right;
            }
          }

          anton::move(left, end_l, out);
          anton::move(right, end_r, out);
        }

        // There are leftover sorted elements. Copy them to the storage buffer
        if(i < length) {
          Random_Access_Iterator b = first + i;
          anton::move(b, last, storage_begin + i);
        }
      } else {
        i64 i = 0;
        for(; i + width < length; i += 2 * width) {
          auto left = storage_begin + i;
          auto right = storage_begin + (i + width);
          auto end_l = right;
          auto end_r = storage_begin + math::min(i + 2 * width, length);
          Random_Access_Iterator out = first + i;
          for(; left != end_l && right != end_r; ++out) {
            bool const greater = p(*right, *left);
            if(!greater) {
              *out = ANTON_MOV(*left);
              ++left;
            } else {
              *out = ANTON_MOV(*right);
              ++right;
            }
          }

          anton::move(left, end_l, out);
          anton::move(right, end_r, out);
        }

        // There are leftover sorted elements. Copy them to the storage buffer
        if(i < length) {
          Random_Access_Iterator out = first + i;
          anton::move(storage_begin + i, storage_end, out);
        }
      }

      copy_a_to_b = !copy_a_to_b;
    }

    // Copy elements back into the input range
    if(!copy_a_to_b) {
      anton::move(storage_begin, storage_end, first);
    }
  }

  template<typename Random_Access_Iterator>
  void merge_sort(Random_Access_Iterator first, Random_Access_Iterator last)
  {
    using value_type =
      typename Iterator_Traits<Random_Access_Iterator>::value_type;
    merge_sort(first, last, [](value_type const& lhs, value_type const& rhs) {
      return lhs < rhs;
    });
  }
} // namespace anton
