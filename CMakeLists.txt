cmake_minimum_required(VERSION 3.10)
project(HATLib)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/HATFormat.cpp
    src/HATEncode.cpp
    src/HATDecode.cpp
)

# Create a static library
add_library(HATLib STATIC ${SOURCES})

# Optionally create a shared library
# add_library(HATLib SHARED ${SOURCES})

# Include directories for public headers
target_include_directories(HATLib PUBLIC include)
