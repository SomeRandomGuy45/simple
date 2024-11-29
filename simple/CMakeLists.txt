cmake_minimum_required(VERSION 3.14)
project(SimpleLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Define source files
set(SRCS
    base_func.cpp
    bytecode.cpp
    helper.cpp
    token.cpp
    vm.cpp
    Node.cpp
    os_windows.cpp
)

# Create the dist directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

# Create a library target based on the platform
set(LIBRARY_NAME simple_main)
add_library(${LIBRARY_NAME} SHARED ${SRCS})

# Set properties for the library
set_target_properties(${LIBRARY_NAME} PROPERTIES
    OUTPUT_NAME "${LIBRARY_NAME}"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)

# Define main executable
add_executable(simple main.cpp)

# Link the library to the executable
target_link_libraries(simple PRIVATE ${LIBRARY_NAME})

# Set output directory for the executable
set_target_properties(simple PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)

if (LINUX)
    target_link_options(simple PRIVATE "-Wl,-rpath,'$ORIGIN'")
endif()

if (APPLE)
add_custom_command(TARGET simple POST_BUILD
    COMMAND install_name_tool -change @rpath/libsimple_main.dylib @executable_path/libsimple_main.dylib
    "${CMAKE_BINARY_DIR}/dist/simple"
)
endif()

if (WIN32)
TARGET_LINK_LIBRARIES(simple PRIVATE gdi32)
endif()

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