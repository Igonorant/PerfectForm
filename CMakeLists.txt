cmake_minimum_required(VERSION 3.26)
project(perfectform C CXX)

# Set the output directory for built objects
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add SDL3 and SDL_image as submodules
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)

# Create the executable
add_executable(perfectform)

# Add source files
target_sources(perfectform
PRIVATE
    src/main.cpp
    src/TextureManager.cpp
    src/TextureManager.h
)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(perfectform PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(perfectform PRIVATE /W4)
endif()

# Link to SDL3 and SDL_image
target_link_libraries(perfectform PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
