cmake_minimum_required(VERSION 3.12)
project(test_system)

set(CMAKE_CXX_STANDARD 14)

INCLUDE_DIRECTORIES(
        lib_thread/
)



add_executable(test_system main.cpp lib_thread/thread_pool.cc lib_thread/task.cc )