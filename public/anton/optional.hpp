#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/assert.hpp>
#include <anton/intrinsics.hpp>
#include <anton/memory.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>

namespace anton {
    struct Null_Optional_Tag {
        explicit Null_Optional_Tag() = default;
    };

    constexpr Null_Optional_Tag null_optional;

    namespace detail {
        template<typename T, bool = is_trivially_destructible<T>>
        class Optional_Destruct_Base {
        public:
            Optional_Destruct_Base(): _null_state(), _holds_value(false) {}
            template<typename... Args>
            Optional_Destruct_Base(Variadic_Construct_Tag, Args&&... args): _value(ANTON_FWD(args)...), _holds_value(true) {}
            ~Optional_Destruct_Base() = default;

            void destruct() {
                if(_holds_value) {
                    _holds_value = false;
                }
            }

            union {
                char _null_state;
                T _value;
            };
            bool _holds_value;
        };

        template<typename T>
        class Optional_Destruct_Base<T, false> {
        public:
            Optional_Destruct_Base(): _null_state(), _holds_value(false) {}

            template<typename... Args>
            Optional_Destruct_Base(Variadic_Construct_Tag, Args&&... args): _value(ANTON_FWD(args)...), _holds_value(true) {}

            ~Optional_Destruct_Base() {
                if(_holds_value) {
                    _value.~T();
                }
            }

            void destruct() {
                if(_holds_value) {
                    _value.~T();
                    _holds_value = false;
                }
            }

            union {
                char _null_state;
                T _value;
            };
            bool _holds_value;
        };

        template<typename T>
        class Optional_Storage_Base: public Optional_Destruct_Base<T> {
        public:
            using Optional_Destruct_Base<T>::Optional_Destruct_Base;

            bool holds_value() const {
                return this->_holds_value;
            }

            T& get() & {
                return this->_value;
            }

            T const& get() const& {
                return this->_value;
            }

            T&& get() && {
                return ANTON_MOV(this->_value);
            }

            T const&& get() const&& {
                return ANTON_MOV(this->_value);
            }

            template<typename... Args>
            void construct(Args&&... args) {
                ANTON_ASSERT(!holds_value(), u8"construct called on empty Optional.");
                ::new(addressof(this->_value)) T(ANTON_FWD(args)...);
                this->_holds_value = true;
            }

            void assign(Optional_Storage_Base const& opt) {
                if(holds_value()) {
                    if(opt.holds_value()) {
                        this->_value = opt.get();
                    } else {
                        this->destruct();
                    }
                } else {
                    if(opt.holds_value()) {
                        construct(opt.get());
                    }
                }
            }

            void assign(Optional_Storage_Base&& opt) {
                if(holds_value()) {
                    if(opt.holds_value()) {
                        this->_value = ANTON_MOV(opt).get();
                        opt.destruct();
                    } else {
                        this->destruct();
                    }
                } else {
                    if(opt.holds_value()) {
                        construct(ANTON_MOV(opt).get());
                        opt.destruct();
                    }
                }
            }
        };

        template<typename T, bool = is_trivially_copy_constructible<T>>
        class Optional_Copy_Base: public Optional_Storage_Base<T> {
        public:
            using Optional_Storage_Base<T>::Optional_Storage_Base;
        };

        template<typename T>
        class Optional_Copy_Base<T, false>: public Optional_Storage_Base<T> {
        public:
            using Optional_Storage_Base<T>::Optional_Storage_Base;

            Optional_Copy_Base() = default;

            Optional_Copy_Base(Optional_Copy_Base const& other) {
                if(other.holds_value()) {
                    construct(other.get());
                }
            }

            Optional_Copy_Base(Optional_Copy_Base&&) noexcept = default;
            Optional_Copy_Base& operator=(Optional_Copy_Base const&) = default;
            Optional_Copy_Base& operator=(Optional_Copy_Base&&) noexcept = default;
        };

        template<typename T, bool = is_trivially_move_constructible<T>>
        class Optional_Move_Base: public Optional_Copy_Base<T> {
        public:
            using Optional_Copy_Base<T>::Optional_Copy_Base;
        };

        template<typename T>
        class Optional_Move_Base<T, false>: public Optional_Copy_Base<T> {
        public:
            using Optional_Copy_Base<T>::Optional_Copy_Base;

            Optional_Move_Base() = default;
            Optional_Move_Base(Optional_Move_Base const&) = default;

            Optional_Move_Base(Optional_Move_Base&& other) noexcept {
                if(other.holds_value()) {
                    construct(ANTON_MOV(other).get());
                    other.destruct();
                }
            }

            Optional_Move_Base& operator=(Optional_Move_Base const&) = default;
            Optional_Move_Base& operator=(Optional_Move_Base&&) noexcept = default;
        };

        template<typename T, bool = is_trivially_copy_assignable<T>>
        class Optional_Copy_Assign_Base: public Optional_Move_Base<T> {
        public:
            using Optional_Move_Base<T>::Optional_Move_Base;
        };

        template<typename T>
        class Optional_Copy_Assign_Base<T, false>: public Optional_Move_Base<T> {
        public:
            using Optional_Move_Base<T>::Optional_Move_Base;

            Optional_Copy_Assign_Base() = default;
            Optional_Copy_Assign_Base(Optional_Copy_Assign_Base const&) = default;
            Optional_Copy_Assign_Base(Optional_Copy_Assign_Base&&) noexcept = default;

            Optional_Copy_Assign_Base& operator=(Optional_Copy_Assign_Base const& other) {
                assign(other);
                return *this;
            }

            Optional_Copy_Assign_Base& operator=(Optional_Copy_Assign_Base&&) noexcept = default;
        };

        template<typename T, bool = is_trivially_move_assignable<T>>
        class Optional_Move_Assign_Base: public Optional_Copy_Assign_Base<T> {
        public:
            using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;
        };

        template<typename T>
        class Optional_Move_Assign_Base<T, false>: public Optional_Copy_Assign_Base<T> {
        public:
            using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;

            Optional_Move_Assign_Base() = default;
            Optional_Move_Assign_Base(Optional_Move_Assign_Base const&) = default;
            Optional_Move_Assign_Base(Optional_Move_Assign_Base&&) noexcept = default;
            Optional_Move_Assign_Base& operator=(Optional_Move_Assign_Base const&) = default;

            Optional_Move_Assign_Base& operator=(Optional_Move_Assign_Base&& other) noexcept {
                assign(ANTON_MOV(other));
                other.destruct();
                return *this;
            }
        };

    } // namespace detail

    // TODO: hashing
    // TODO: emplace
    template<typename T>
    class Optional: private detail::Optional_Move_Assign_Base<T> {
    private:
        using _base = detail::Optional_Move_Assign_Base<T>;

        template<typename U>
        struct Is_Constructible_From_Type {
            static constexpr bool value =
                conjunction<Negation<Is_Same<Optional<T>, remove_const_ref<U>>>, Negation<Is_Same<Variadic_Construct_Tag, remove_const_ref<U>>>,
                            Negation<Is_Same<Null_Optional_Tag, remove_const_ref<U>>>, Is_Constructible<T, U>>;
        };

    public:
        using value_type = T;

        Optional(Null_Optional_Tag) {}

        template<typename... Args, typename = enable_if<is_constructible<T, Args...>>>
        explicit Optional(Variadic_Construct_Tag, Args&&... args): _base(variadic_construct, ANTON_FWD(args)...) {}

        template<typename U>
        Optional(U&& arg, enable_if<Is_Constructible_From_Type<U&&>::value && is_convertible<U&&, T>, int> = 0): _base(variadic_construct, ANTON_FWD(arg)) {}

        template<typename U>
        explicit Optional(U&& arg, enable_if<Is_Constructible_From_Type<U&&>::value && !is_convertible<U&&, T>, int> = 0)
            : _base(variadic_construct, ANTON_FWD(arg)) {}

        Optional(Optional const&) = default;
        // Leaves other in null_optional state
        Optional(Optional&& other) noexcept = default;
        Optional& operator=(Optional const&) = default;
        // Leaves other in null_optional state
        Optional& operator=(Optional&& other) noexcept = default;
        ~Optional() = default;

        [[nodiscard]] operator bool() const {
            return _base::holds_value();
        }

        [[nodiscard]] bool holds_value() const {
            return _base::holds_value();
        }

        [[nodiscard]] T* operator->() {
            ANTON_ASSERT(holds_value(), u8"operator-> called on empty Optional.");
            return addressof(this->get());
        }

        [[nodiscard]] T const* operator->() const {
            ANTON_ASSERT(holds_value(), u8"operator-> called on empty Optional.");
            return addressof(this->get());
        }

        [[nodiscard]] T& operator*() & {
            ANTON_ASSERT(holds_value(), u8"operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T const& operator*() const& {
            ANTON_ASSERT(holds_value(), u8"operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T&& operator*() && {
            ANTON_ASSERT(holds_value(), u8"operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T const&& operator*() const&& {
            ANTON_ASSERT(holds_value(), u8"operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T& value() & {
            if constexpr(ANTON_CHECK_OPTIONAL_VALUE) {
                ANTON_FAIL(!holds_value(), u8"value() called on empty Optional.");
            }
            return this->get();
        }

        [[nodiscard]] T const& value() const& {
            if constexpr(ANTON_CHECK_OPTIONAL_VALUE) {
                ANTON_FAIL(!holds_value(), u8"value() called on empty Optional.");
            }
            return this->get();
        }

        [[nodiscard]] T&& value() && {
            if constexpr(ANTON_CHECK_OPTIONAL_VALUE) {
                ANTON_FAIL(!holds_value(), u8"value() called on empty Optional.");
            }
            return this->get();
        }

        [[nodiscard]] T const&& value() const&& {
            if constexpr(ANTON_CHECK_OPTIONAL_VALUE) {
                ANTON_FAIL(!holds_value(), u8"value() called on empty Optional.");
            }
            return this->get();
        }
    };

    template<typename T>
    inline void swap(Optional<T>& lhs, Optional<T>& rhs) {
        if(lhs.holds_value()) {
            if(rhs.holds_value()) {
                swap(*lhs, *rhs);
            } else {
                rhs = ANTON_MOV(lhs);
            }
        } else {
            if(rhs.holds_value()) {
                lhs = ANTON_MOV(rhs);
            }
        }
    }
} // namespace anton
