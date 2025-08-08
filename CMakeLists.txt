cmake_minimum_required(VERSION 3.10)
project(MyJsonApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pkg-config
find_package(PkgConfig REQUIRED)

# Use pkg-config to find jsoncpp
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Add your executable
add_executable(MyJsonApp main.cpp)

# Include directories and link flags from pkg-config
target_include_directories(MyJsonApp PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(MyJsonApp PRIVATE ${JSONCPP_LIBRARIES})

# Optionally add compile definitions and flags
target_compile_definitions(MyJsonApp PRIVATE ${JSONCPP_CFLAGS_OTHER})

set(CMAKE_CXX_FLAGS_DEBUG "-g3")
