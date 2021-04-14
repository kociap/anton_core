#pragma once

#include <anton/functors.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits/properties.hpp>
#include <anton/type_traits/transformations.hpp>
#include <anton/types.hpp>
#include <anton/utility.hpp>

namespace anton {
    // Owning_Ptr
    // Owns and manages the lifetime of an allocated object.
    // An owning pointer might manage no object, i.e. wa default constructed or or the pointer was set to nullptr.
    // The object is destroyed using either a user-supplied deleter or the delete operator if the user did not provide a deleter.
    // Deleter is a function pointer. Stateful deleters are not suporrted.
    //
    // TODO: Fix new/deallocate mismatch
    //
    template<typename T>
    class Owning_Ptr {
    public:
        using element_type = T;
        using deleter_type = void (*)(T*);

    private:
        T* _pointer = nullptr;
        deleter_type _deleter = nullptr;

    public:
        Owning_Ptr() = default;

        Owning_Ptr(T* pointer): _pointer(pointer) {}

        Owning_Ptr(T* pointer, deleter_type deleter): _pointer(pointer), _deleter(deleter) {}

        template<typename... Args>
        Owning_Ptr(Variadic_Construct_Tag tag, Args&&... args): _pointer(new T(ANTON_FWD(args)...)) {}

        template<typename... Args>
        Owning_Ptr(deleter_type deleter, Variadic_Construct_Tag tag, Args&&... args): _pointer(new T(ANTON_FWD(args)...)), _deleter(deleter) {}

        Owning_Ptr(Owning_Ptr const& other) = delete;

        Owning_Ptr(Owning_Ptr&& other): _pointer(other._pointer), _deleter(other._deleter) {
            other._pointer = nullptr;
            other._deleter = nullptr;
        }

        Owning_Ptr& operator=(Owning_Ptr const& other) = delete;

        Owning_Ptr& operator=(Owning_Ptr&& other) {
            swap(*this, other);
            return *this;
        }

        ~Owning_Ptr() {
            if(_deleter) {
                _deleter(_pointer);
            } else {
                if constexpr(is_complete_type<T>) {
                    destruct(_pointer);
                }

                deallocate(_pointer);
            }
        }

        [[nodiscard]] operator bool() const {
            return _pointer;
        }

        [[nodiscard]] add_lvalue_reference<T> operator*() const {
            return *_pointer;
        }

        [[nodiscard]] T* operator->() const {
            return _pointer;
        }

        [[nodiscard]] T* get_ptr() const {
            return _pointer;
        }

        [[nodiscard]] deleter_type get_deleter() const {
            return _deleter;
        }

        // replace
        // Replace the pointer to the managed object with another one.
        //
        // Returns:
        // Previously owned pointer.
        //
        [[nodiscard]] T* replace(T* pointer) {
            T* old = _pointer;
            _pointer = pointer;
            return old;
        }

        friend void swap(Owning_Ptr& lhs, Owning_Ptr& rhs) {
            swap(lhs._pointer, rhs._pointer);
            swap(lhs._deleter, rhs._deleter);
        }
    };

    template<typename T>
    [[nodiscard]] bool operator==(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() == rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator!=(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() != rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator<(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() < rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator>(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() > rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator<=(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() <= rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator>=(Owning_Ptr<T> const& lhs, Owning_Ptr<T> const& rhs) {
        return lhs.get_ptr() >= rhs.get_ptr();
    }
} // namespace anton
