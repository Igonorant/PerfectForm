cmake_minimum_required(VERSION 3.26)
project(perfectform C CXX)

find_package(SDL3 REQUIRED)

add_executable(perfectform)

target_sources(perfectform
PRIVATE
    src/main.cpp
)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(perfectform PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(perfectform PRIVATE /W4)
endif()

target_link_libraries(perfectform SDL3::SDL3)
