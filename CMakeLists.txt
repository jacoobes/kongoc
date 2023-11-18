# CMake minimum version required
cmake_minimum_required(VERSION 3.19)

# Project name
project(kongoc)

# Set C++ standard (change as needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src/)
add_subdirectory(test)
