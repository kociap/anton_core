cmake_minimum_required(VERSION 3.13)

option(ANTON_BUILD_DEBUG "Whether to build debug" ON)
option(ANTON_UNICODE_VALIDATE_ENCODING "Enable unicode validation" OFF)
option(ANTON_STRING_VIEW_VERIFY_ENCODING "Enable unicode validation in String_View" OFF)
option(ANTON_ITERATOR_DEBUG "Enable iterator checking" OFF)
option(ANTON_DISABLE_VERIFY "Disable ANTON_VERIFY" OFF)
option(ANTON_CHECK_OPTIONAL_VALUE "Enable checking whether optional holds a value" OFF)

add_library(anton_core)
target_include_directories(anton_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_sources(anton_core 
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/crt.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/string_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_function.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_properties.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_transformations.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/traits_utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/detail/utility_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/hashing/murmurhash2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/unicode/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/algorithm.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/aligned_buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/allocator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/compiletime.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/diagnostic_macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/fixed_array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/flat_hash_map.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/functors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/intrinsics.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/iterators.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/optional.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/pair.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/shared_ptr.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/slice.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/string_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/tags.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/type_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/vector.hpp"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/private/allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string_common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/unicode/common.cpp"
)

if(ANTON_UNICODE_VALIDATE_ENCODING)
    target_compile_definitions(anton_core PRIVATE ANTON_UNICODE_VALIDATE_ENCODING=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_UNICODE_VALIDATE_ENCODING=0)
endif()

if(ANTON_ITERATOR_DEBUG)
    target_compile_definitions(anton_core PRIVATE ANTON_ITERATOR_DEBUG=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_ITERATOR_DEBUG=0)
endif()

if(ANTON_STRING_VIEW_VERIFY_ENCODING)
    target_compile_definitions(anton_core PRIVATE ANTON_STRING_VIEW_VERIFY_ENCODING=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_STRING_VIEW_VERIFY_ENCODING=0)
endif()

if(ANTON_BUILD_DEBUG)
    target_compile_definitions(anton_core PRIVATE ANTON_DEBUG=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_DEBUG=0)
endif()

if(ANTON_DISABLE_VERIFY)
    target_compile_definitions(anton_core PRIVATE ANTON_DISABLE_VERIFY=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_DISABLE_VERIFY=0)
endif()

if(ANTON_CHECK_OPTIONAL_VALUE)
    target_compile_definitions(anton_core PRIVATE ANTON_CHECK_OPTIONAL_VALUE=1)
else()
    target_compile_definitions(anton_core PRIVATE ANTON_CHECK_OPTIONAL_VALUE=0)
endif()

add_subdirectory(dependencies/anton_math)
target_link_libraries(anton_core PUBLIC anton_math)
