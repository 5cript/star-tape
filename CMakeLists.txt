# Version check
cmake_minimum_required (VERSION 3.0)

# Project
project(star-tape)

# Add files
file(GLOB sources "*.cpp" "tape_io/*.cpp")

# Add library
add_library(star-tape STATIC ${sources})

# Compiler Options
target_compile_options(star-tape PRIVATE -fexceptions -std=c++14 -O2 -Wall -pedantic-errors -pedantic)