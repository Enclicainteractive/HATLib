cmake_minimum_required(VERSION 3.10)

project(HATEncoder)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../HATLib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(HATEncoder ${SOURCES})

# Link with HATLib
target_link_libraries(HATEncoder HATLib)

# Installation rules
install(TARGETS HATEncoder
    RUNTIME DESTINATION bin
)
