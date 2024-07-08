cmake_minimum_required(VERSION 3.28)
project(locks-usage)

set(CMAKE_CXX_STANDARD 17)

add_executable(sloppy-counter sloppy-counter.cpp)
add_executable(counter counter.cpp)
add_executable(queue queue.cpp)
add_executable(list list.cpp)
