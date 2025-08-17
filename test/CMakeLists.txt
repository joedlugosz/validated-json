cmake_minimum_required(VERSION 3.10)
project(ValidatedJsonTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

option(CODE_COVERAGE "Enable coverage reporting" OFF)

if(CODE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Code coverage enabled")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(FATAL_ERROR "Code coverage only supported with GCC/Clang")
    endif()
endif()
enable_testing()

add_executable(ValidatedJsonTest test.cpp ../ValidatedJson.cpp)

target_include_directories(ValidatedJsonTest PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(ValidatedJsonTest PRIVATE gtest gtest_main ${JSONCPP_LIBRARIES})
target_compile_definitions(ValidatedJsonTest PRIVATE ${JSONCPP_CFLAGS_OTHER})

include(CTest)
include(GoogleTest)
gtest_discover_tests(ValidatedJsonTest)

configure_file(${CMAKE_SOURCE_DIR}/test.json ${CMAKE_BINARY_DIR}/test.json COPYONLY)
