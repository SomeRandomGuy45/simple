#!/bin/bash

# Change to the specified executable's directory
cd "$1"

# Update the dynamic library paths for the executable
install_name_tool -change @rpath/libsimple_main.dylib @executable_path/libsimple_main.dylib "$2"