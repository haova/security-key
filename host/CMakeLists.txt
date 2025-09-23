cmake_minimum_required(VERSION 3.13)

project(host C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Find hidapi (modern CMake has a package config, otherwise fallback)
find_package(hidapi REQUIRED)

add_executable(host src/main.c)

# Link against hidapi
target_link_libraries(host PRIVATE hidapi::hidapi)
