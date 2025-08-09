cmake_minimum_required(VERSION 3.10)
project(MyJsonApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(MyJsonApp main.cpp ValidatedJson.cpp)

target_include_directories(MyJsonApp PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(MyJsonApp PRIVATE ${JSONCPP_LIBRARIES})
target_compile_definitions(MyJsonApp PRIVATE ${JSONCPP_CFLAGS_OTHER})

set(CMAKE_CXX_FLAGS_DEBUG "-g3")
