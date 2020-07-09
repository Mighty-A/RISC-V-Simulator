cmake_minimum_required(VERSION 3.10)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)

add_executable(RISC_V main.cpp memory.hpp register.hpp instruction.hpp type.hpp)