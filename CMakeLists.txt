cmake_minimum_required(VERSION 3.26)
project(perfectform C CXX)

find_package(SDL3 REQUIRED)

add_executable(perfectform)

target_sources(perfectform
PRIVATE
    src/snake.cpp
)

target_link_libraries(perfectform SDL3::SDL3)
