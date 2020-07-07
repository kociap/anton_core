#pragma once

#include <anton/memory.hpp>
#include <anton/type_traits.hpp>

namespace anton {
    struct Expected_Error_Tag {
        explicit Expected_Error_Tag() = default;
    };

    constexpr Expected_Error_Tag expected_error;

    struct Expected_Value_Tag {
        explicit Expected_Value_Tag() = default;
    };

    constexpr Expected_Value_Tag expected_value;

    template<typename T, typename E>
    class Expected {
    public:
        using value_type = T;
        using error_type = E;

        template<typename... Args>
        Expected(Expected_Value_Tag, Args&&... args): _holds_value(true), _value{anton::forward<Args&&>(args)...} {}

        template<typename... Args>
        Expected(Expected_Error_Tag, Args&&... args): _holds_value(false), _error{anton::forward<Args&&>(args)...} {}

        Expected(Expected const& other): _holds_value(other._holds_value), _null_state() {
            if(other._holds_value) {
                anton::construct(anton::addressof(_value), other._value);
            } else {
                anton::construct(anton::addressof(_error), other._error);
            }
        }

        Expected(Expected&& other): _holds_value(other._holds_value), _null_state() {
            if(other._holds_value) {
                anton::construct(anton::addressof(_value), anton::move(other._value));
            } else {
                anton::construct(anton::addressof(_error), anton::move(other._error));
            }
        }

        Expected& operator=(Expected const& other) {
            if(_holds_value) {
                if(other._holds_value) {
                    _value = other._value;
                } else {
                    anton::destruct(anton::addressof(_value));
                    anton::construct(anton::addressof(_error), other._error);
                    _holds_value = false;
                }
            } else {
                if(other._holds_value) {
                    anton::destruct(anton::addressof(_error));
                    anton::construct(anton::addressof(_value), other._value);
                    _holds_value = true;
                } else {
                    _error = other._error;
                }
            }
        }

        Expected& operator=(Expected&& other) {
            if(_holds_value) {
                if(other._holds_value) {
                    _value = anton::move(other._value);
                } else {
                    anton::destruct(anton::addressof(_value));
                    anton::construct(anton::addressof(_error), anton::move(other._error));
                    _holds_value = false;
                }
            } else {
                if(other._holds_value) {
                    anton::destruct(anton::addressof(_error));
                    anton::construct(anton::addressof(_value), anton::move(other._value));
                    _holds_value = true;
                } else {
                    _error = anton::move(other._error);
                }
            }
        }

        ~Expected() {
            if(_holds_value) {
                _value.~T();
            } else {
                _error.~E();
            }
        }

        [[nodiscard]] operator bool() const {
            return _holds_value;
        }

        [[nodiscard]] bool holds_value() const {
            return _holds_value;
        }

        [[nodiscard]] T* operator->() {
            return &_value;
        }

        [[nodiscard]] T const* operator->() const {
            return &_value;
        }

        [[nodiscard]] T& operator*() & {
            return _value;
        }

        [[nodiscard]] T const& operator*() const& {
            return _value;
        }

        [[nodiscard]] T&& operator*() && {
            return _value;
        }

        [[nodiscard]] T const&& operator*() const&& {
            return _value;
        }

        [[nodiscard]] T& value() & {
            return _value;
        }

        [[nodiscard]] T const& value() const& {
            return _value;
        }

        [[nodiscard]] T&& value() && {
            return _value;
        }

        [[nodiscard]] T const&& value() const&& {
            return _value;
        }

        [[nodiscard]] E& error() & {
            return _error;
        }

        [[nodiscard]] E const& error() const& {
            return _error;
        }

        [[nodiscard]] E&& error() && {
            return _error;
        }

        [[nodiscard]] E const&& error() const&& {
            return _error;
        }

        friend void swap(Expected& lhs, Expected& rhs) {
            if(lhs._holds_value) {
                if(rhs._holds_value) {
                    swap(lhs._value, rhs._value);
                } else {
                    anton::construct(anton::addressof(rhs._value), anton::move(lhs._value));
                    anton::destruct(anton::addressof(lhs._value));
                    anton::construct(anton::addressof(lhs._error), anton::move(rhs._error));
                    anton::destruct(anton::addressof(rhs._error));
                    anton::swap(lhs._holds_value, rhs._holds_value);
                }
            } else {
                if(rhs._holds_value) {
                    anton::construct(anton::addressof(rhs._error), anton::move(lhs._error));
                    anton::destruct(anton::addressof(lhs._error));
                    anton::construct(anton::addressof(lhs._value), anton::move(rhs._value));
                    anton::destruct(anton::addressof(rhs._value));
                    anton::swap(lhs._holds_value, rhs._holds_value);
                } else {
                    swap(lhs._error, rhs._error);
                }
            }
        }

    private:
        bool _holds_value;
        union {
            T _value;
            E _error;
            bool _null_state;
        };
    };

    template<typename E>
    class Expected<void, E> {
    public:
        using value_type = void;
        using error_type = E;

        Expected(Expected_Value_Tag): _holds_value(true) {}

        template<typename... Args>
        Expected(Expected_Error_Tag, Args&&... args): _holds_value(false), _error{anton::forward<Args&&>(args)...} {}

        Expected(Expected const& other): _holds_value(other._holds_value), _null_state() {
            if(!other._holds_value) {
                anton::construct(anton::addressof(_error), other._error);
            }
        }

        Expected(Expected&& other): _holds_value(other._holds_value), _null_state() {
            if(!other._holds_value) {
                anton::construct(anton::addressof(_error), anton::move(other._error));
            }
        }

        Expected& operator=(Expected const& other) {
            if(_holds_value) {
                if(!other._holds_value) {
                    anton::construct(anton::addressof(_error), other._error);
                    anton::destruct(anton::addressof(other._error));
                    _holds_value = false;
                }
            } else {
                if(other._holds_value) {
                    anton::destruct(anton::addressof(_error));
                    _holds_value = true;
                } else {
                    _error = other._error;
                }
            }
        }

        Expected& operator=(Expected&& other) {
            if(_holds_value) {
                if(!other._holds_value) {
                    anton::construct(anton::addressof(_error), anton::move(other._error));
                    anton::destruct(anton::addressof(other._error));
                    _holds_value = false;
                }
            } else {
                if(other._holds_value) {
                    anton::destruct(anton::addressof(_error));
                    _holds_value = true;
                } else {
                    _error = anton::move(other._error);
                }
            }
        }

        ~Expected() {
            if(!_holds_value) {
                _error.~E();
            }
        }

        [[nodiscard]] operator bool() const {
            return _holds_value;
        }

        [[nodiscard]] bool holds_value() const {
            return _holds_value;
        }

        [[nodiscard]] E& error() & {
            return _error;
        }

        [[nodiscard]] E const& error() const& {
            return _error;
        }

        [[nodiscard]] E&& error() && {
            return _error;
        }

        [[nodiscard]] E const&& error() const&& {
            return _error;
        }

        friend void swap(Expected& lhs, Expected& rhs) {
            if(lhs._holds_value) {
                if(!rhs._holds_value) {
                    anton::construct(anton::addressof(lhs._error), anton::move(rhs._error));
                    anton::destruct(anton::addressof(rhs._error));
                    swap(lhs._holds_value, rhs._holds_value);
                }
            } else {
                if(rhs._holds_value) {
                    anton::construct(anton::addressof(rhs._error), anton::move(lhs._error));
                    anton::destruct(anton::addressof(lhs._error));
                    swap(lhs._holds_value, rhs._holds_value);
                }
            }
        }

    private:
        bool _holds_value;
        union {
            E _error;
            bool _null_state;
        };
    };
} // namespace anton
