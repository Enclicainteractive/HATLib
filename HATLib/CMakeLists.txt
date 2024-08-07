cmake_minimum_required(VERSION 3.10)

project(HATLib)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}../lib/lz4/lib)

# Source files
set(SOURCES
    src/HATFormat.cpp
    src/HATEncode.cpp
    src/HATDecode.cpp
    ../lib/lz4/lib/lz4.c
    ../lib/lz4/lib/lz4hc.c
)

# Create static library
add_library(HATLib STATIC ${SOURCES})

# Installation rules
install(TARGETS HATLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
