cmake_minimum_required(VERSION 3.15)

option(ANTON_BUILD_DEBUG "Whether to build debug" ON)
option(ANTON_UNICODE_VALIDATE_ENCODING "Enable unicode validation" OFF)
option(ANTON_STRING_VIEW_VERIFY_ENCODING "Enable unicode validation in anton::String_View" OFF)
option(ANTON_STRING_VERIFY_ENCODING "Enable unicode validation in anton::String" OFF)
option(ANTON_ITERATOR_DEBUG "Enable iterator checking" OFF)
option(ANTON_DISABLE_VERIFY "Disable ANTON_VERIFY" OFF)
option(ANTON_CHECK_OPTIONAL_VALUE "Enable checking whether optional holds a value" OFF)
option(ANTON_UNREACHABLE_ASSERTS "ANTON_UNREACHABLE will use assert instead of an intrinsic" OFF)

include(FetchContent)

# Add anton_math
FetchContent_Declare(
    anton_math
    GIT_REPOSITORY https://github.com/kociap/anton_math.git
)
FetchContent_MakeAvailable(anton_math)

project(anton_core)

add_library(anton_core)
set_target_properties(anton_core PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
target_include_directories(anton_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_sources(anton_core 
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/algorithm/sort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/crt.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/detail/string_common.hpp"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/iterators.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/optional.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/owning_ptr.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/pair.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/slice.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/stacktrace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/tags.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/type_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/typeid.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/anton/array.hpp"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/private/allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/format.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/stacktrace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_stream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_view.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/unicode/common.cpp"
)

target_compile_definitions(anton_core PRIVATE _CRT_SECURE_NO_WARNINGS)

if(ANTON_UNICODE_VALIDATE_ENCODING)
    target_compile_definitions(anton_core PUBLIC ANTON_UNICODE_VALIDATE_ENCODING=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_UNICODE_VALIDATE_ENCODING=0)
endif()

if(ANTON_ITERATOR_DEBUG)
    target_compile_definitions(anton_core PUBLIC ANTON_ITERATOR_DEBUG=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_ITERATOR_DEBUG=0)
endif()

if(ANTON_STRING_VIEW_VERIFY_ENCODING)
    target_compile_definitions(anton_core PUBLIC ANTON_STRING_VIEW_VERIFY_ENCODING=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_STRING_VIEW_VERIFY_ENCODING=0)
endif()

if(ANTON_STRING_VERIFY_ENCODING)
    target_compile_definitions(anton_core PUBLIC ANTON_STRING_VERIFY_ENCODING=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_STRING_VERIFY_ENCODING=0)
endif()

if(ANTON_BUILD_DEBUG)
    target_compile_definitions(anton_core PUBLIC ANTON_DEBUG=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_DEBUG=0)
endif()

if(ANTON_DISABLE_VERIFY)
    target_compile_definitions(anton_core PUBLIC ANTON_DISABLE_VERIFY=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_DISABLE_VERIFY=0)
endif()

if(ANTON_CHECK_OPTIONAL_VALUE)
    target_compile_definitions(anton_core PUBLIC ANTON_CHECK_OPTIONAL_VALUE=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_CHECK_OPTIONAL_VALUE=0)
endif()

if(ANTON_UNREACHABLE_ASSERTS)
    target_compile_definitions(anton_core PUBLIC ANTON_UNREACHABLE_ASSERTS=1)
else()
    target_compile_definitions(anton_core PUBLIC ANTON_UNREACHABLE_ASSERTS=0)
endif()

target_link_libraries(anton_core PUBLIC anton_math PRIVATE DbgHelp)
