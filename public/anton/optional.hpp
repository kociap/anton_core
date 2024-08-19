#pragma once

#include <anton/assert.hpp>
#include <anton/intrinsics.hpp>
#include <anton/memory/core.hpp>
#include <anton/swap.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits.hpp>

namespace anton {
  struct Null_Optional_Tag {
    explicit Null_Optional_Tag() = default;
  };

  constexpr Null_Optional_Tag null_optional;

  namespace detail {
    template<typename T, bool Reference = is_reference<T>,
             bool Trivially_Destructible = is_trivially_destructible<T>>
    struct Optional_Storage_Base;

    template<typename T>
    struct Optional_Storage_Base<T, false, true> {
    private:
      union {
        char _null_state;
        T _value;
      };
      bool _holds_value;

    public:
      Optional_Storage_Base(): _null_state(), _holds_value(false) {}
      template<typename... Args>
      Optional_Storage_Base(Variadic_Construct_Tag, Args&&... args)
        : _value(ANTON_FWD(args)...), _holds_value(true)
      {
      }
      ~Optional_Storage_Base() = default;

      [[nodiscard]] bool holds_value() const
      {
        return _holds_value;
      }

      [[nodiscard]] T& get() &
      {
        return _value;
      }

      [[nodiscard]] T const& get() const&
      {
        return _value;
      }

      [[nodiscard]] T&& get() &&
      {
        return ANTON_MOV(_value);
      }

      [[nodiscard]] T const&& get() const&&
      {
        return ANTON_MOV(_value);
      }

      template<typename... Args>
      void construct(Args&&... args)
      {
        ANTON_ASSERT(!holds_value(), u8"construct called on engaged Optional");
        ::new(ANTON_ADDRESSOF(_value)) T(ANTON_FWD(args)...);
        _holds_value = true;
      }

      template<typename That>
      void assign_from(That&& optional)
      {
        if(holds_value()) {
          if(optional.holds_value()) {
            _value = ANTON_FWD(optional).get();
          } else {
            reset();
          }
        } else {
          if(optional.holds_value()) {
            construct(ANTON_FWD(optional).get());
          }
        }
      }

      void reset()
      {
        if(_holds_value) {
          _holds_value = false;
        }
      }
    };

    template<typename T>
    struct Optional_Storage_Base<T, false, false> {
    private:
      union {
        char _null_state;
        T _value;
      };
      bool _holds_value;

    public:
      Optional_Storage_Base(): _null_state(), _holds_value(false) {}
      template<typename... Args>
      Optional_Storage_Base(Variadic_Construct_Tag, Args&&... args)
        : _value(ANTON_FWD(args)...), _holds_value(true)
      {
      }
      ~Optional_Storage_Base()
      {
        reset();
      }

      [[nodiscard]] bool holds_value() const
      {
        return _holds_value;
      }

      [[nodiscard]] T& get() &
      {
        return _value;
      }

      [[nodiscard]] T const& get() const&
      {
        return _value;
      }

      [[nodiscard]] T&& get() &&
      {
        return ANTON_MOV(_value);
      }

      [[nodiscard]] T const&& get() const&&
      {
        return ANTON_MOV(_value);
      }

      template<typename... Args>
      void construct(Args&&... args)
      {
        ANTON_ASSERT(!holds_value(), u8"construct called on engaged Optional");
        ::new(ANTON_ADDRESSOF(_value)) T(ANTON_FWD(args)...);
        _holds_value = true;
      }

      template<typename That>
      void assign_from(That&& optional)
      {
        if(holds_value()) {
          if(optional.holds_value()) {
            _value = ANTON_FWD(optional).get();
          } else {
            reset();
          }
        } else {
          if(optional.holds_value()) {
            construct(ANTON_FWD(optional).get());
          }
        }
      }

      void reset()
      {
        if(_holds_value) {
          _value.~T();
          _holds_value = false;
        }
      }
    };

    template<typename T>
    struct Optional_Storage_Base<T, true, true> {
    private:
      using _raw_type = remove_reference<T>;

      _raw_type* _value = nullptr;

    public:
      Optional_Storage_Base() = default;
      template<typename Arg>
      Optional_Storage_Base(Variadic_Construct_Tag, Arg&& args)
        : _value(ANTON_ADDRESSOF(args))
      {
      }
      ~Optional_Storage_Base() = default;

      [[nodiscard]] bool holds_value() const
      {
        return _value != nullptr;
      }

      [[nodiscard]] T& get() &
      {
        return *_value;
      }

      [[nodiscard]] T const& get() const&
      {
        return *_value;
      }

      [[nodiscard]] T&& get() &&
      {
        return ANTON_MOV(*_value);
      }

      [[nodiscard]] T const&& get() const&&
      {
        return ANTON_MOV(*_value);
      }

      template<typename Args>
      void construct(Args&& args)
      {
        ANTON_ASSERT(!holds_value(), u8"construct called on engaged Optional");
        _value = ANTON_ADDRESSOF(args);
      }

      void assign(Optional_Storage_Base const& opt)
      {
        if(holds_value()) {
          if(opt.holds_value()) {
            *_value = opt.get();
          } else {
            reset();
          }
        } else {
          if(opt.holds_value()) {
            construct(opt.get());
          }
        }
      }

      void assign(Optional_Storage_Base&& opt)
      {
        if(holds_value()) {
          if(opt.holds_value()) {
            *_value = ANTON_MOV(opt).get();
            opt.reset();
          } else {
            reset();
          }
        } else {
          if(opt.holds_value()) {
            construct(ANTON_MOV(opt).get());
          }
        }
      }

      template<typename That>
      void assign_from(That&& optional)
      {
        if(holds_value()) {
          if(optional.holds_value()) {
            _value = ADDRESSOF(ANTON_FWD(optional).get());
          } else {
            reset();
          }
        } else {
          if(optional.holds_value()) {
            construct(ANTON_FWD(optional).get());
          }
        }
      }

      void reset()
      {
        _value = nullptr;
      }
    };

    template<typename T, bool = is_trivially_copy_constructible<T>>
    struct Optional_Copy_Base: public Optional_Storage_Base<T> {
    public:
      using Optional_Storage_Base<T>::Optional_Storage_Base;
    };

    template<typename T>
    struct Optional_Copy_Base<T, false>: public Optional_Storage_Base<T> {
    public:
      using Optional_Storage_Base<T>::Optional_Storage_Base;

      Optional_Copy_Base() = default;
      ~Optional_Copy_Base() = default;

      Optional_Copy_Base(Optional_Copy_Base const& other)
      {
        if(other.holds_value()) {
          this->construct(other.get());
        }
      }

      Optional_Copy_Base(Optional_Copy_Base&&) = default;
      Optional_Copy_Base& operator=(Optional_Copy_Base const&) = default;
      Optional_Copy_Base& operator=(Optional_Copy_Base&&) = default;
    };

    template<typename T, bool = is_trivially_move_constructible<T>>
    struct Optional_Move_Base: public Optional_Copy_Base<T> {
    public:
      using Optional_Copy_Base<T>::Optional_Copy_Base;
    };

    template<typename T>
    struct Optional_Move_Base<T, false>: public Optional_Copy_Base<T> {
    public:
      using Optional_Copy_Base<T>::Optional_Copy_Base;

      Optional_Move_Base() = default;
      ~Optional_Move_Base() = default;
      Optional_Move_Base(Optional_Move_Base const&) = default;

      Optional_Move_Base(Optional_Move_Base&& other)
      {
        if(other.holds_value()) {
          this->construct(ANTON_MOV(other).get());
        }
      }

      Optional_Move_Base& operator=(Optional_Move_Base const&) = default;
      Optional_Move_Base& operator=(Optional_Move_Base&&) = default;
    };

    template<typename T, bool = is_trivially_copy_assignable<T>>
    struct Optional_Copy_Assign_Base: public Optional_Move_Base<T> {
    public:
      using Optional_Move_Base<T>::Optional_Move_Base;
    };

    template<typename T>
    struct Optional_Copy_Assign_Base<T, false>: public Optional_Move_Base<T> {
    public:
      using Optional_Move_Base<T>::Optional_Move_Base;

      Optional_Copy_Assign_Base() = default;
      ~Optional_Copy_Assign_Base() = default;
      Optional_Copy_Assign_Base(Optional_Copy_Assign_Base const&) = default;
      Optional_Copy_Assign_Base(Optional_Copy_Assign_Base&&) = default;

      Optional_Copy_Assign_Base&
      operator=(Optional_Copy_Assign_Base const& other)
      {
        this->assign_from(other);
        return *this;
      }

      Optional_Copy_Assign_Base&
      operator=(Optional_Copy_Assign_Base&&) = default;
    };

    template<typename T, bool = is_trivially_move_assignable<T>>
    struct Optional_Move_Assign_Base: public Optional_Copy_Assign_Base<T> {
    public:
      using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;
    };

    template<typename T>
    struct Optional_Move_Assign_Base<T, false>
      : public Optional_Copy_Assign_Base<T> {
    public:
      using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;

      Optional_Move_Assign_Base() = default;
      ~Optional_Move_Assign_Base() = default;
      Optional_Move_Assign_Base(Optional_Move_Assign_Base const&) = default;
      Optional_Move_Assign_Base(Optional_Move_Assign_Base&&) = default;
      Optional_Move_Assign_Base&
      operator=(Optional_Move_Assign_Base const&) = default;

      Optional_Move_Assign_Base& operator=(Optional_Move_Assign_Base&& other)
      {
        this->assign_from(ANTON_MOV(other));
        return *this;
      }
    };

  } // namespace detail

  // TODO: hashing
  // TODO: emplace
  template<typename T>
  struct Optional: private detail::Optional_Move_Assign_Base<T> {
  private:
    using _base = detail::Optional_Move_Assign_Base<T>;

    template<typename U>
    struct Is_Constructible_From_Type {
      static constexpr bool value = conjunction<
        Negation<Is_Same<Optional<T>, remove_const_ref<U>>>,
        Negation<Is_Same<Variadic_Construct_Tag, remove_const_ref<U>>>,
        Negation<Is_Same<Null_Optional_Tag, remove_const_ref<U>>>,
        Is_Constructible<T, U>>;
    };

  public:
    using value_type = T;
    using pointer_type = add_pointer<remove_reference<T>>;
    using pointer_const_type = add_pointer<remove_reference<T> const>;

    Optional(Null_Optional_Tag) {}

    template<typename... Args,
             typename = enable_if<is_constructible<T, Args...>>>
    explicit Optional(Variadic_Construct_Tag, Args&&... args)
      : _base(variadic_construct, ANTON_FWD(args)...)
    {
    }

    template<typename U>
    Optional(U&& arg, enable_if<Is_Constructible_From_Type<U&&>::value &&
                                  is_convertible<U&&, T>,
                                int> = 0)
      : _base(variadic_construct, ANTON_FWD(arg))
    {
    }

    template<typename U>
    explicit Optional(U&& arg,
                      enable_if<Is_Constructible_From_Type<U&&>::value &&
                                  !is_convertible<U&&, T>,
                                int> = 0)
      : _base(variadic_construct, ANTON_FWD(arg))
    {
    }

    Optional(Optional const&) = default;
    // Does not change other.has_value().
    Optional(Optional&& other) = default;
    Optional& operator=(Optional const&) = default;
    // Does not change other.has_value().
    Optional& operator=(Optional&& other) = default;
    ~Optional() = default;

    using _base::holds_value;
    using _base::reset;

    [[nodiscard]] operator bool() const
    {
      return holds_value();
    }

    [[nodiscard]] pointer_type operator->()
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(),
                   u8"operator-> called on disengaged Optional.");
      }
      return ANTON_ADDRESSOF(this->get());
    }

    [[nodiscard]] pointer_const_type operator->() const
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(),
                   u8"operator-> called on disengaged Optional.");
      }
      return ANTON_ADDRESSOF(this->get());
    }

    [[nodiscard]] T& operator*() &
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"operator* called on disengaged Optional.");
      }
      return this->get();
    }

    [[nodiscard]] T const& operator*() const&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"operator* called on disengaged Optional.");
      }
      return this->get();
    }

    [[nodiscard]] T&& operator*() &&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"operator* called on disengaged Optional.");
      }
      return this->get();
    }

    [[nodiscard]] T const&& operator*() const&&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"operator* called on disengaged Optional.");
      }
      return this->get();
    }

    [[nodiscard]] T& value() &
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"value() called on disengaged Optional");
      }
      return this->get();
    }

    [[nodiscard]] T const& value() const&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"value() called on disengaged Optional");
      }
      return this->get();
    }

    [[nodiscard]] T&& value() &&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"value() called on disengaged Optional");
      }
      return this->get();
    }

    [[nodiscard]] T const&& value() const&&
    {
      if constexpr(ANTON_OPTIONAL_CHECK_VALUE) {
        ANTON_FAIL(holds_value(), u8"value() called on disengaged Optional");
      }
      return this->get();
    }
  };

  template<typename T>
  inline void swap(Optional<T>& lhs, Optional<T>& rhs)
  {
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
