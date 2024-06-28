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

# Create a shared library (DLL)
add_library(HATLib SHARED ${SOURCES})

# Define export macro for Windows
if(WIN32)
    target_compile_definitions(HATLib PRIVATE HATLIB_EXPORTS)
endif()

# Include directories for public headers
target_include_directories(HATLib PUBLIC include)

# Installation rules
install(TARGETS HATLib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
