cmake_minimum_required(VERSION 3.12)
project(asyncio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(io_multiplexing io_multiplexing.cpp)
add_executable(multithreading multithreading.cpp)
# add_executable(coroutine coroutine.cpp)
