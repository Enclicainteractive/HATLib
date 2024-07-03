cmake_minimum_required(VERSION 3.10)

project(HATEdit)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../HATLib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(HATEdit ${SOURCES})

# Link with HATLib
target_link_libraries(HATEdit HATLib)

# Installation rules
install(TARGETS HATEdit
    RUNTIME DESTINATION bin
)
