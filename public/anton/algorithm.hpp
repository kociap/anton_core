#pragma once

#include <anton/array.hpp>
#include <anton/iterators.hpp>
#include <anton/math/math.hpp>
#include <anton/memory.hpp>
#include <anton/pair.hpp>
#include <anton/utility.hpp>

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

    template<typename Forward_Iterator, typename Predicate>
    Forward_Iterator is_sorted(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
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
    // predicate   - comparison function. Returns true when the first argument is less than (i.e. ordered before) the second argument.
    //               The function must not modify the objects passed to it.
    //
    template<typename Forward_Iterator, typename Predicate>
    void bubble_sort(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
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
    void bubble_sort(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        bubble_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
    }

    template<typename Random_Access_Iterator, typename Predicate>
    void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last, Predicate predicate) {
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
    void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last) {
        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
        insertion_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
    }

    template<typename Random_Access_Iterator, typename Predicate>
    void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last, Predicate predicate) {
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
    void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last) {
        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
        quick_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
    }

    // merge_sort
    // Stable non-in-place sort. Allocates additional memory proportional to last - first.
    //
    // Parameters:
    // first, last - the range to sort.
    // predicate   - comparison function. Returns true when the first argument is less than (i.e. ordered before) the second argument.
    //               The function must not modify the objects passed to it.
    //
    template<typename Random_Access_Iterator, typename Predicate>
    void merge_sort(Random_Access_Iterator first, Random_Access_Iterator last, Predicate p) {
        i64 const length = last - first;
        // Sort 4-long runs using bubble sort
        for(i64 i = 0; true; i += 4) {
            Random_Access_Iterator b = first + i;
            Random_Access_Iterator e = first + math::min(i + 4, length);
            bubble_sort(b, e, p);
            if(i + 4 >= length) {
                break;
            }
        }

        if(length <= 4) {
            return;
        }

        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
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
                    Random_Access_Iterator end_r = first + math::min(i + 2 * width, length);
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
    void merge_sort(Random_Access_Iterator first, Random_Access_Iterator last) {
        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
        merge_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
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
