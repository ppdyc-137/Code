cmake_minimum_required(VERSION 3.28)
project(condition_variables)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(condition_variables cv.cpp)
add_executable(semaphore semaphore.cpp)
