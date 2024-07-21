cmake_minimum_required(VERSION 3.30)

project(gimi VERSION 0.1.0 LANGUAGES C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS ./src/*.c ./src/*.h)

configure_file(src/gimi_constants.h.in src/gimi_constants.h)

add_executable(gimi ${SRC})
