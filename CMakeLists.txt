cmake_minimum_required(VERSION 3.30)

project(gimi VERSION 0.1.0 LANGUAGES C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB_RECURSE SRC ./src/*.c ./src/*.h)

add_executable(gimi ${SRC})
