#pragma once

#include <anton/type_traits/transformations.hpp>

#define ANTON_MOV(x) (static_cast<anton::remove_reference<decltype(x)>&&>(x))
#define ANTON_FWD(x) (static_cast<decltype(x)&&>(x))
#define ANTON_AS_CONST(x) (static_cast<anton::add_const<decltype(x)>&>(x))
