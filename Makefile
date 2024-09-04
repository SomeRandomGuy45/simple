# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++2a -Wall -g

# Directories
BUILD_DIR = build
LIB_DIR = $(CURDIR)/libs

# File names
MAIN_SRC = main.cpp
LIB_SRC = $(LIB_DIR)/main_lib.cpp

# Determine the shared library extension based on the operating system
ifeq ($(OS),Windows_NT)
    LIB_EXT = dll
else
    ifeq ($(shell uname -s),Darwin)
        LIB_EXT = dylib
    else
        LIB_EXT = so
    endif
endif

# Object files
INTERPRETER_OBJ = $(BUILD_DIR)/interpreter.o
MAIN_OBJ = $(BUILD_DIR)/main.o

# Output files
EXECUTABLE = $(BUILD_DIR)/simple
LIBRARY = $(LIB_DIR)/libmain_lib.$(LIB_EXT)

# Build rules
all: $(BUILD_DIR) $(LIBRARY) $(EXECUTABLE)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Build the library
$(LIBRARY): $(LIB_SRC)
	$(CXX) -shared -fPIC -o $(LIBRARY) $(LIB_SRC) $(CXXFLAGS)

# Build the executable
$(EXECUTABLE): $(INTERPRETER_OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(INTERPRETER_OBJ) $(MAIN_OBJ) -L$(LIB_DIR) -lmain_lib -Wl,-rpath,$(LIB_DIR)

# Object files
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIBRARY)

.PHONY: all clean
