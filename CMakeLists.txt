cmake_minimum_required(VERSION 3.22.0)
project(Parallel-Ray-Tracer VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -Wshadow -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories("include/")
set(SOURCE_FILES src/main.cpp)

add_executable(ray-tracer "${SOURCE_FILES}")
