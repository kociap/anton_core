#pragma once

#include <anton/assert.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>
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
  struct Expected {
  public:
    using value_type = T;
    using error_type = E;

    template<typename... Args>
    Expected(Expected_Value_Tag, Args&&... args)
      : _value{ANTON_FWD(args)...}, _holds_value(true)
    {
    }

    template<typename... Args>
    Expected(Expected_Error_Tag, Args&&... args)
      : _error{ANTON_FWD(args)...}, _holds_value(false)
    {
    }

    Expected(Expected const& other)
      : _null_state(), _holds_value(other._holds_value)
    {
      if(other._holds_value) {
        anton::construct(anton::addressof(_value), other._value);
      } else {
        anton::construct(anton::addressof(_error), other._error);
      }
    }

    Expected(Expected&& other): _null_state(), _holds_value(other._holds_value)
    {
      if(other._holds_value) {
        anton::construct(anton::addressof(_value), ANTON_MOV(other._value));
      } else {
        anton::construct(anton::addressof(_error), ANTON_MOV(other._error));
      }
    }

    Expected& operator=(Expected const& other)
    {
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

      return *this;
    }

    Expected& operator=(Expected&& other)
    {
      if(_holds_value) {
        if(other._holds_value) {
          _value = ANTON_MOV(other._value);
        } else {
          anton::destruct(anton::addressof(_value));
          anton::construct(anton::addressof(_error), ANTON_MOV(other._error));
          _holds_value = false;
        }
      } else {
        if(other._holds_value) {
          anton::destruct(anton::addressof(_error));
          anton::construct(anton::addressof(_value), ANTON_MOV(other._value));
          _holds_value = true;
        } else {
          _error = ANTON_MOV(other._error);
        }
      }

      return *this;
    }

    ~Expected()
    {
      if(_holds_value) {
        _value.~T();
      } else {
        _error.~E();
      }
    }

    [[nodiscard]] operator bool() const
    {
      return _holds_value;
    }

    [[nodiscard]] bool holds_value() const
    {
      return _holds_value;
    }

    [[nodiscard]] T* operator->()
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator-> on Expected that does not hold a value");
      return &_value;
    }

    [[nodiscard]] T const* operator->() const
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator-> on Expected that does not hold a value");
      return &_value;
    }

    [[nodiscard]] T& operator*() &
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator* on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T const& operator*() const&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator* on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T&& operator*() &&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator* on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T const&& operator*() const&&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call operator* on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T& value() &
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call value() on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T const& value() const&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call value() on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T&& value() &&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call value() on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] T const&& value() const&&
    {
      ANTON_ASSERT(
        _holds_value,
        u8"cannot call value() on Expected that does not hold a value");
      return _value;
    }

    [[nodiscard]] E& error() &
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E const& error() const&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E&& error() &&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E const&& error() const&&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    friend void swap(Expected& lhs, Expected& rhs)
    {
      if(lhs._holds_value) {
        if(rhs._holds_value) {
          swap(lhs._value, rhs._value);
        } else {
          anton::construct(anton::addressof(rhs._value), ANTON_MOV(lhs._value));
          anton::destruct(anton::addressof(lhs._value));
          anton::construct(anton::addressof(lhs._error), ANTON_MOV(rhs._error));
          anton::destruct(anton::addressof(rhs._error));
          anton::swap(lhs._holds_value, rhs._holds_value);
        }
      } else {
        if(rhs._holds_value) {
          anton::construct(anton::addressof(rhs._error), ANTON_MOV(lhs._error));
          anton::destruct(anton::addressof(lhs._error));
          anton::construct(anton::addressof(lhs._value), ANTON_MOV(rhs._value));
          anton::destruct(anton::addressof(rhs._value));
          anton::swap(lhs._holds_value, rhs._holds_value);
        } else {
          swap(lhs._error, rhs._error);
        }
      }
    }

  private:
    union {
      T _value;
      E _error;
      bool _null_state;
    };
    bool _holds_value;
  };

  template<typename E>
  struct Expected<void, E> {
  public:
    using value_type = void;
    using error_type = E;

    Expected(Expected_Value_Tag): _null_state(), _holds_value(true) {}

    template<typename... Args>
    Expected(Expected_Error_Tag, Args&&... args)
      : _error{ANTON_FWD(args)...}, _holds_value(false)
    {
    }

    Expected(E const& v): _error{v}, _holds_value(false) {}
    Expected(E&& v): _error{ANTON_MOV(v)}, _holds_value(false) {}

    Expected(Expected const& other)
      : _null_state(), _holds_value(other._holds_value)
    {
      if(!other._holds_value) {
        anton::construct(anton::addressof(_error), other._error);
      }
    }

    Expected(Expected&& other): _null_state(), _holds_value(other._holds_value)
    {
      if(!other._holds_value) {
        anton::construct(anton::addressof(_error), ANTON_MOV(other._error));
      }
    }

    Expected& operator=(Expected const& other)
    {
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

      return *this;
    }

    Expected& operator=(Expected&& other)
    {
      if(_holds_value) {
        if(!other._holds_value) {
          anton::construct(anton::addressof(_error), ANTON_MOV(other._error));
          anton::destruct(anton::addressof(other._error));
          _holds_value = false;
        }
      } else {
        if(other._holds_value) {
          anton::destruct(anton::addressof(_error));
          _holds_value = true;
        } else {
          _error = ANTON_MOV(other._error);
        }
      }

      return *this;
    }

    ~Expected()
    {
      if(!_holds_value) {
        _error.~E();
      }
    }

    [[nodiscard]] operator bool() const
    {
      return _holds_value;
    }

    [[nodiscard]] bool holds_value() const
    {
      return _holds_value;
    }

    [[nodiscard]] E& error() &
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E const& error() const&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E&& error() &&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    [[nodiscard]] E const&& error() const&&
    {
      ANTON_ASSERT(
        !_holds_value,
        u8"cannot call error() on Expected that does not hold a value");
      return _error;
    }

    friend void swap(Expected& lhs, Expected& rhs)
    {
      if(lhs._holds_value) {
        if(!rhs._holds_value) {
          anton::construct(anton::addressof(lhs._error), ANTON_MOV(rhs._error));
          anton::destruct(anton::addressof(rhs._error));
          swap(lhs._holds_value, rhs._holds_value);
        }
      } else {
        if(rhs._holds_value) {
          anton::construct(anton::addressof(rhs._error), ANTON_MOV(lhs._error));
          anton::destruct(anton::addressof(lhs._error));
          swap(lhs._holds_value, rhs._holds_value);
        }
      }
    }

  private:
    union {
      E _error;
      bool _null_state;
    };
    bool _holds_value;
  };
} // namespace anton
