
cmake_minimum_required(VERSION 3.10)
project(Scope2025)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use default compiler
# Remove explicit compiler specification

# Add compile options for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wpedantic")

# Create executable
add_executable(code main.cpp)
