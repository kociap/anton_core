cmake_minimum_required(VERSION 3.15)

option(ANTON_BUILD_DEBUG "Whether to build debug" OFF)
option(ANTON_UNICODE_VALIDATE_ENCODING "Enable unicode validation" OFF)
option(ANTON_STRING_VIEW_VERIFY_ENCODING "Enable unicode validation in anton::String_View" OFF)
option(ANTON_STRING_VERIFY_ENCODING "Enable unicode validation in anton::String" OFF)
option(ANTON_ITERATOR_DEBUG "Enable iterator checking" OFF)
option(ANTON_DISABLE_VERIFY "Disable ANTON_VERIFY" OFF)
option(ANTON_OPTIONAL_CHECK_VALUE "Enable checking whether optional holds a value" OFF)
option(ANTON_UNREACHABLE_ASSERTS "ANTON_UNREACHABLE will use assert instead of an intrinsic" OFF)

include(FetchContent)

# Add anton_math
FetchContent_Declare(
    anton_math
    GIT_REPOSITORY https://github.com/kociap/anton_math.git
    GIT_TAG f69f9521cacb1a39fef72f136c3424cff8db09b4
)
FetchContent_MakeAvailable(anton_math)

project(anton_core)

# Detect OS
SET(ANTON_LINUX OFF)
SET(ANTON_WINDOWS OFF)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    SET(ANTON_LINUX ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    SET(ANTON_WINDOWS ON)
else()
    message(FATAL_ERROR "unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

add_library(anton_core)
set_target_properties(anton_core PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
target_include_directories(anton_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_sources(anton_core
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/crt.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/string_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/string8_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/swap.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/function.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/properties.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/transformations.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits/utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/hashing/murmurhash2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators/move.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators/range.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators/reverse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators/zip.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/memory/core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/unicode/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/algorithm.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/aligned_buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/allocator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/compiletime.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/console.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/diagnostic_macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/expected.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/filesystem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/fixed_array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/flat_hash_map.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/flat_hash_set.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/format.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/functors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/intrinsics.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/intrusive_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/optional.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/owning_ptr.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/pair.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/slice.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/sort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/stacktrace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string7_stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string7_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string7.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/tags.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/typeid.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/array.hpp"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/private/allocator/allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/allocator/arena.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/format.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/intrusive_list.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_stream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_view.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string7_stream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string7_view.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string7.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string8_common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/unicode/common.cpp"
)

if(ANTON_LINUX)
    target_sources(anton_core
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/private/linux/stacktrace.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/linux/filesystem.cpp"
    )

    target_link_libraries(anton_core PUBLIC anton_math)
endif()

if(ANTON_WINDOWS)
    target_sources(anton_core
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/private/windows/stacktrace.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/windows/filesystem.cpp"
    )

    target_compile_definitions(anton_core
        PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        VC_EXTRALEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )

    target_link_libraries(anton_core PUBLIC anton_math PRIVATE DbgHelp)
endif()

target_compile_definitions(anton_core PUBLIC
    ANTON_BUILD_DEBUG=$<BOOL:${ANTON_BUILD_DEBUG}>
    ANTON_ITERATOR_DEBUG=$<BOOL:${ANTON_ITERATOR_DEBUG}>
    ANTON_DISABLE_VERIFY=$<BOOL:${ANTON_DISABLE_VERIFY}>
    ANTON_UNREACHABLE_ASSERTS=$<BOOL:${ANTON_UNREACHABLE_ASSERTS}>
    ANTON_UNICODE_VALIDATE_ENCODING=$<BOOL:${ANTON_UNICODE_VALIDATE_ENCODING}>
    ANTON_STRING_VIEW_VERIFY_ENCODING=$<BOOL:${ANTON_STRING_VIEW_VERIFY_ENCODING}>
    ANTON_STRING_VERIFY_ENCODING=$<BOOL:${ANTON_STRING_VERIFY_ENCODING}>
    ANTON_OPTIONAL_CHECK_VALUE=$<BOOL:${ANTON_OPTIONAL_CHECK_VALUE}>
)

# Detect compilers

# TODO: Change detection to use CMAKE_CXX_COMPILER_ID and CMAKE_CXX_COMPILER_FRONTEND_VARIANT

# clang++ (CLANG)
string(FIND "${CMAKE_CXX_COMPILER}" "clang++" ANTON_COMPILER_CLANGPP)
if(ANTON_COMPILER_CLANGPP GREATER -1)
  set(ANTON_COMPILER_CLANGPP ON)
else()
  set(ANTON_COMPILER_CLANGPP OFF)
endif()

# clang-cl (CLANG)
string(FIND "${CMAKE_CXX_COMPILER}" "clang-cl" ANTON_COMPILER_CLANGCL)
if(ANTON_COMPILER_CLANGCL GREATER -1)
  set(ANTON_COMPILER_CLANGCL ON)
else()
  set(ANTON_COMPILER_CLANGCL OFF)
endif()

if(ANTON_COMPILER_CLANGPP OR ANTON_COMPILER_CLANGCL)
  set(ANTON_COMPILER_CLANG ON)
else()
  set(ANTON_COMPILER_CLANG OFF)
endif()

# GNU g++
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(ANTON_COMPILER_GPP ON)
else()
  set(ANTON_COMPILER_GPP OFF)
endif()

# cl (MSVC)
string(FIND "${CMAKE_CXX_COMPILER}" "cl" ANTON_COMPILER_CL)
if(ANTON_COMPILER_CL GREATER -1)
  set(ANTON_COMPILER_CL ON)
else()
  set(ANTON_COMPILER_CL OFF)
endif()

# TODO: Add appropriate flags, e.g. no exceptions, no rtti, etc.

if(NOT ANTON_COMPILER_CLANG AND NOT ANTON_COMPILER_GPP AND NOT ANTON_COMPILER_CL)
  message(FATAL_ERROR "unrecognized compiler '${CMAKE_CXX_COMPILER}' with ID '${CMAKE_CXX_COMPILER_ID}'")
endif()

target_compile_definitions(anton_core
  PUBLIC
  ANTON_COMPILER_CLANG=$<BOOL:${ANTON_COMPILER_CLANG}>
  ANTON_COMPILER_GPP=$<BOOL:${ANTON_COMPILER_GPP}>
  ANTON_COMPILER_CL=$<BOOL:${ANTON_COMPILER_CL}>
)
