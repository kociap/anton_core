#pragma once

#include <anton/detail/crt.hpp>
#include <anton/iterators.hpp>
#include <anton/memory/core.hpp>
#include <anton/type_traits.hpp>

// TODO: add constexpr if for trivial operations.

namespace anton {
    template<typename Forward_Iterator>
    void destruct([[maybe_unused]] Forward_Iterator first, [[maybe_unused]] Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!is_trivially_destructible<value_type>) {
            for(; first != last; ++first) {
                first->~value_type();
            }
        }
    }

    template<typename Forward_Iterator, typename Count>
    void destruct_n([[maybe_unused]] Forward_Iterator first, [[maybe_unused]] Count n) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!is_trivially_destructible<value_type>) {
            for(; n > 0; --n, ++first) {
                first->~value_type();
            }
        }
    }

    template<typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        for(; first != last; ++first, ++dest) {
            anton::construct(ANTON_ADDRESSOF(*dest), *first);
        }
        return dest;
    }

    template<typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        for(; n > 0; --n, ++first, ++dest) {
            anton::construct(ANTON_ADDRESSOF(*dest), *first);
        }
        return dest;
    }

    template<typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_move(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        for(; first != last; ++first, ++dest) {
            anton::construct(ANTON_ADDRESSOF(*dest), ANTON_MOV(*first));
        }
        return dest;
    }

    template<typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_move_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        for(; n > 0; --n, ++first, ++dest) {
            anton::construct(ANTON_ADDRESSOF(*dest), ANTON_MOV(*first));
        }
        return dest;
    }

    template<typename Forward_Iterator>
    void uninitialized_default_construct(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!is_trivially_constructible<value_type>) {
            for(; first != last; ++first) {
                anton::construct(ANTON_ADDRESSOF(*first));
            }
        } else {
            for(; first != last; ++first) {
                *first = {};
            }
        }
    }

    template<typename Forward_Iterator, typename Count>
    void uninitialized_default_construct_n(Forward_Iterator first, Count n) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!is_trivially_constructible<value_type>) {
            for(; n > 0; --n, ++first) {
                anton::construct(ANTON_ADDRESSOF(*first));
            }
        } else {
            for(; n > 0; --n, ++first) {
                *first = {};
            }
        }
    }

    template<typename Forward_Iterator, typename T>
    void uninitialized_fill(Forward_Iterator first, Forward_Iterator last, T const& val) {
        for(; first != last; ++first) {
            anton::construct(ANTON_ADDRESSOF(*first), val);
        }
    }

    template<typename Forward_Iterator, typename Count, typename T>
    void uninitialized_fill_n(Forward_Iterator first, Count n, T const& val) {
        for(; n > 0; --n, ++first) {
            anton::construct(ANTON_ADDRESSOF(*first), val);
        }
    }

    template<typename Forward_Iterator, typename... Ts>
    void uninitialized_variadic_construct(Forward_Iterator first, Ts&&... vals) {
        (..., anton::construct(ANTON_ADDRESSOF(*(first++)), ANTON_FWD(vals)));
    }

    // copy
    // Copy assigns elements from [first, last[ to dest.
    //
    // Returns:
    // An iterator to the end of the dest range.
    //
    template<typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator copy(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            conjunction<Is_Iterator_Category<input_iterator_cat, Contiguous_Iterator_Tag>, Is_Iterator_Category<output_iterator_cat, Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && is_trivial<input_value_type> && is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for(; first != last; ++first, ++dest) {
                *dest = *first;
            }
            return dest;
        }
    }

    // copy_backward
    //
    // Copy assigns elements from range [first, last[ to dest in reverse order (last element is copied first) decrementing dest after each copy.
    // The back of the range [first, last[ may overlap the front of the range [dest - (first - last), dest[.
    //
    // Returns:
    // An iterator to the beginning of the destination range.
    //
    // Example usage:
    //   copy_backward(array.begin(), array.end(), dest_array.end());
    //
    // Reference implementation:
    //   template<typename Bidirectional_Iterator>
    //   Bidirectional_Iterator copy_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Bidirectional_Iterator dest) {
    //       while (last != first) {
    //           *--dest = *--last;
    //       }
    //       return dest;
    //   }
    //
    template<typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator copy_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            conjunction<Is_Iterator_Category<input_iterator_cat, Contiguous_Iterator_Tag>, Is_Iterator_Category<output_iterator_cat, Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && is_trivial<input_value_type> && is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while(last != first) {
                --dest;
                --last;
                *dest = *last;
            }
            return dest;
        }
    }

    // move
    // Move assigns elements from [first, last[ to dest.
    //
    // Returns:
    // An iterator to the end of the dest range.
    //
    template<typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator move(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            conjunction<Is_Same<input_iterator_cat, Contiguous_Iterator_Tag>, Is_Same<output_iterator_cat, Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && is_trivial<input_value_type> && is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for(; first != last; ++first, ++dest) {
                *dest = ANTON_MOV(*first);
            }
            return dest;
        }
    }

    // move_backward
    //
    // Move assigns elements from range [first, last[ to dest in reverse order (last element is copied first) decrementing dest after each move.
    // The back of the range [first, last[ may overlap the front of the range [dest - (first - last), dest[.
    //
    // Returns:
    // An iterator to the beginning of the destination range.
    //
    // Example usage:
    //   move_backward(array.begin(), array.end(), dest_array.end());
    //
    // Reference implementation:
    //   template<typename Bidirectional_Iterator>
    //   Bidirectional_Iterator move_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Bidirectional_Iterator dest) {
    //       while (last != first) {
    //           *--dest = ANTON_MOV(*--last);
    //       }
    //       return dest;
    //   }
    //
    template<typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator move_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            conjunction<Is_Iterator_Category<input_iterator_cat, Contiguous_Iterator_Tag>, Is_Iterator_Category<output_iterator_cat, Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && is_trivial<input_value_type> && is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while(last != first) {
                --dest;
                --last;
                *dest = ANTON_MOV(*last);
            }
            return dest;
        }
    }
} // namespace anton
