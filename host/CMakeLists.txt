cmake_minimum_required(VERSION 3.13)

project(host C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(host src/main.c)
