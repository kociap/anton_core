#pragma once

#include <anton/algorithm/sort.hpp>
#include <anton/array.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/pair.hpp>
#include <anton/swap.hpp>

namespace anton {
    // fill_with_consecutive
    // Fills range [first, last[ with consecutive values starting at value.
    //
    template<typename Forward_Iterator, typename T>
    void fill_with_consecutive(Forward_Iterator first, Forward_Iterator last, T value) {
        for(; first != last; ++first, ++value) {
            *first = value;
        }
    }

    // find
    // Linearily searches the range [first, last[.
    //
    // Returns: The iterator to the first element in the range [first, last[ that satisfies
    // the condition *iterator == value or last if no such iterator is found.
    //
    // Complexity: At most 'last - first' comparisons.
    //
    template<typename Input_Iterator, typename T>
    [[nodiscard]] Input_Iterator find(Input_Iterator first, Input_Iterator last, T const& value) {
        while(first != last && *first != value) {
            ++first;
        }
        return first;
    }

    // find_if
    // Linearily searches the range [first, last[.
    //
    // Returns: The iterator to the first element in the range [first, last[ that satisfies
    // the condition predicate(*iterator) == true or last if no such iterator is found.
    //
    // Complexity: At most 'last - first' applications of the predicate.
    //
    template<typename Input_Iterator, typename Predicate>
    [[nodiscard]] Input_Iterator find_if(Input_Iterator first, Input_Iterator last, Predicate predicate) {
        while(first != last && !predicate(*first)) {
            ++first;
        }
        return first;
    }

    template<typename Input_Iterator, typename Predicate>
    [[nodiscard]] bool any_of(Input_Iterator first, Input_Iterator last, Predicate predicate) {
        for(; first != last; ++first) {
            if(predicate(*first)) {
                return true;
            }
        }
        return false;
    }

    // rotate_left
    // Performs left rotation on the range by swapping elements so that middle becomes the first element.
    // [first, last[ must be a valid range, middle must be within [first, last[.
    //
    // This function works on forward iterators meaning it supports lists, however you can get much better
    // performance by using splice operations instead (O(1) instead of O(n)).
    //
    // Returns: Position of the first element after rotate.
    //
    // Complexity: At most last - first swaps.
    //
    template<typename Forward_Iterator>
    Forward_Iterator rotate_left(Forward_Iterator first, Forward_Iterator middle, Forward_Iterator last) {
        using anton::swap;
        Forward_Iterator i = middle;
        while(true) {
            swap(*first, *i);
            ++first;
            ++i;

            if(i == last) {
                break;
            }

            if(first == middle) {
                middle = i;
            }
        }

        Forward_Iterator r = first;
        if(first != middle) {
            i = middle;
            while(true) {
                swap(*first, *i);
                ++first;
                ++i;
                if(i == last) {
                    if(first == middle) {
                        break;
                    }
                    i = middle;
                } else if(first == middle) {
                    middle = i;
                }
            }
        }
        return r;
    }

    // unique
    // Eliminates all except the first element from every consecutive group of equivalent elements from
    // the range [first, last[ and returns a past-the-end iterator for the new logical end of the range.
    //
    template<typename Forward_Iterator, typename Predicate>
    Forward_Iterator unique(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
        if(first == last) {
            return last;
        }

        // Find first group that consists of > 2 duplicates.
        Forward_Iterator next = first;
        ++next;
        while(next != last && !predicate(*first, *next)) {
            ++first;
            ++next;
        }

        if(next != last) {
            for(; next != last; ++next) {
                if(!predicate(*first, *next)) {
                    ++first;
                    *first = ANTON_MOV(*next);
                }
            }
            return ++first;
        } else {
            return last;
        }
    }

    // unique
    // Eliminates all except the first element from every consecutive group of equivalent elements from
    // the range [first, last[ and returns a past-the-end iterator for the new logical end of the range.
    //
    template<typename Forward_Iterator>
    Forward_Iterator unique(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        return unique(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs == rhs; });
    }

    // set_difference
    // Copies elements from [first1, last1[ that are not present in [first2, last2[ to
    // the destination range starting at dest.
    //
    // Requires:
    // Both input ranges must be sorted and neither must overlap with destination range.
    // (*first1 < *first2) and (*first2 < *first1) must be valid expressions.
    //
    // Returns: The end of the destination range.
    //
    // Complexity: TODO
    //
    template<typename Input_Iterator1, typename Input_Iterator2, typename Output_Iterator>
    Output_Iterator set_difference(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2, Input_Iterator2 last2, Output_Iterator dest) {
        while(first1 != last1 && first2 != last2) {
            if(*first1 < *first2) {
                *dest = *first1;
                ++dest;
                ++first1;
            } else if(*first2 < *first1) {
                ++first2;
            } else {
                ++first1;
                ++first2;
            }
        }

        for(; first1 != last1; ++first1, ++dest) {
            *dest = *first1;
        }

        return dest;
    }

    // set_difference overload with custom comparison predicate which enforces strict ordering (<).
    //
    template<typename Input_Iterator1, typename Input_Iterator2, typename Output_Iterator, typename Compare>
    Output_Iterator set_difference(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2, Input_Iterator2 last2, Output_Iterator dest,
                                   Compare compare) {
        while(first1 != last1 && first2 != last2) {
            if(compare(*first1, *first2)) {
                *dest = *first1;
                ++dest;
                ++first1;
            } else if(compare(*first2, *first1)) {
                ++first2;
            } else {
                ++first1;
                ++first2;
            }
        }

        for(; first1 != last1; ++first1, ++dest) {
            *dest = *first1;
        }

        return dest;
    }

    // mismatch
    // Finds the first mismatching pair of elements in the two ranges
    // defined by [first1, last1[ and [first2, first2 + (last1 - first1)[.
    // The elements are compared using operator==.
    //
    // Parameters:
    // first1, last1 - first range of elements.
    // first2, last2 - seconds range of elements.
    //             p - predicate that returns true when the elements should be treated as equal.
    //                 The signature should be equivalent to bool(Type1 const&, Type2 const&)
    //
    // Returns:
    // Pair of iterators to the first elements that are not equal.
    // If no mismatches are found, one or both iterators are the end iterators.
    //
    template<typename Input_Iterator1, typename Input_Iterator2>
    Pair<Input_Iterator1, Input_Iterator2> mismatch(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2) {
        while(first1 != last1 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    // mismatch
    // Finds the first mismatching pair of elements in the two ranges
    // defined by [first1, last1[ and [first2, first2 + (last1 - first1)[.
    // The elements are compared using the predicate p.
    //
    // Parameters:
    // first1, last1 - first range of elements.
    // first2, last2 - seconds range of elements.
    //             p - predicate that returns true when the elements should be treated as equal.
    //                 The signature should be equivalent to bool(Type1 const&, Type2 const&)
    //
    // Returns:
    // Pair of iterators to the first elements that are not equal.
    // If no mismatches are found, one or both iterators are the end iterators.
    //
    template<typename Input_Iterator1, typename Input_Iterator2, typename Predicate>
    Pair<Input_Iterator1, Input_Iterator2> mismatch(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2, Predicate p) {
        while(first1 != last1 && p(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    // mismatch
    // Finds the first mismatching pair of elements in the two ranges defined by [first1, last1[ and [first2, last2[.
    // The elements are compared using operator==.
    //
    // Parameters:
    // first1, last1 - first range of elements.
    // first2, last2 - seconds range of elements.
    //
    // Returns:
    // Pair of iterators to the first elements that are not equal.
    // If no mismatches are found, one or both iterators are the end iterators.
    //
    template<typename Input_Iterator1, typename Input_Iterator2>
    Pair<Input_Iterator1, Input_Iterator2> mismatch(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2, Input_Iterator2 last2) {
        while(first1 != last1 && first2 != last2 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    // mismatch
    // Finds the first mismatching pair of elements in the two ranges defined by [first1, last1[ and [first2, last2[.
    // The elements are compared using the predicate p.
    //
    // Parameters:
    // first1, last1 - first range of elements.
    // first2, last2 - seconds range of elements.
    //             p - predicate that returns true when the elements should be treated as equal.
    //                 The signature should be equivalent to bool(Type1 const&, Type2 const&)
    //
    // Returns:
    // Pair of iterators to the first elements that are not equal.
    // If no mismatches are found, one or both iterators are the end iterators.
    //
    template<typename Input_Iterator1, typename Input_Iterator2, typename Predicate>
    Pair<Input_Iterator1, Input_Iterator2> mismatch(Input_Iterator1 first1, Input_Iterator1 last1, Input_Iterator2 first2, Input_Iterator2 last2, Predicate p) {
        while(first1 != last1 && first2 != last2 && p(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }
} // namespace anton
