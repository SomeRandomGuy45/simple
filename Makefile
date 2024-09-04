# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++2a -Wall -g

# Directories
BUILD_DIR = build
LIB_DIR = $(CURDIR)/libs

# File names
INTERPRETER_SRC = interpreter.cpp
MAIN_SRC = main.cpp
LIB_SRC = $(LIB_DIR)/main_lib.cpp

# Determine the shared library extension based on the operating system
ifeq ($(OS),Windows_NT)
    LIB_EXT = dll
	FOLDER = $(shell mkdir $(BUILD_DIR))
else
	FOLDER = $(shell mkdir -p $(BUILD_DIR))
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIB_EXT = dylib
    else
        LIB_EXT = so
    endif
endif

# Object files
INTERPRETER_OBJ = interpreter.o
MAIN_OBJ = main.o

# Output files
EXECUTABLE = simple
LIBRARY = $(LIB_DIR)/libmain_lib.$(LIB_EXT)

# Build rules
all: $(LIBRARY) $(BUILD_DIR)/$(EXECUTABLE)

# Build the library
$(LIBRARY): $(LIB_SRC)
	$(CXX) -shared -fPIC -o $(LIBRARY) $(LIB_SRC) $(CXXFLAGS)

# Build the executable
$(BUILD_DIR)/$(EXECUTABLE): $(BUILD_DIR)/$(INTERPRETER_OBJ) $(BUILD_DIR)/$(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(BUILD_DIR)/$(INTERPRETER_OBJ) $(BUILD_DIR)/$(MAIN_OBJ) -L$(LIB_DIR) -lmain_lib -Wl,-rpath,$(LIB_DIR)

# Object files
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIBRARY)

.PHONY: all clean
