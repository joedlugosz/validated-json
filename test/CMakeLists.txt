cmake_minimum_required(VERSION 3.10)
project(ValidatedJsonTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

add_executable(ValidatedJsonTest test.cpp ../ValidatedJson.cpp)
target_link_libraries(ValidatedJsonTest PRIVATE Catch2::Catch2WithMain)

target_include_directories(ValidatedJsonTest PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(ValidatedJsonTest PRIVATE ${JSONCPP_LIBRARIES})
target_compile_definitions(ValidatedJsonTest PRIVATE ${JSONCPP_CFLAGS_OTHER})

include(CTest)
include(Catch)
catch_discover_tests(ValidatedJsonTest)

configure_file(${CMAKE_SOURCE_DIR}/test.json ${CMAKE_BINARY_DIR}/test.json COPYONLY)
