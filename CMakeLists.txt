cmake_minimum_required(VERSION 3.14)
project(SimpleLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fvisibility=default")

# Define source files
set(SRCS
    base_func.cpp
    bytecode.cpp
    helper.cpp
    token.cpp
    vm.cpp
    Node.cpp
)

# Create the dist directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

# Create a library target based on the platform
set(LIBRARY_NAME simple_main)  # Removed the 'lib' prefix for the output
add_library(${LIBRARY_NAME} SHARED ${SRCS})

# Set the output directory for the library
set_target_properties(${LIBRARY_NAME} PROPERTIES
    OUTPUT_NAME "${LIBRARY_NAME}"  # Ensuring the output name has 'lib' prefix
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)

# Define main executable
add_executable(simple main.cpp)

# Link the library to the executable
target_link_libraries(simple PRIVATE ${LIBRARY_NAME})

# Specify output directories for the executable
set_target_properties(simple PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)

# Clean target
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${CMAKE_BINARY_DIR}/dist/libsimple.dll
    ${CMAKE_BINARY_DIR}/dist/libsimple.dylib
    ${CMAKE_BINARY_DIR}/dist/libsimple.so
)

# Add a custom target for running the program
add_custom_target(run
    COMMAND $<TARGET_FILE:simple>
    DEPENDS simple
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)