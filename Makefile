# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -g

# Directories
BUILD_DIR = build
LIB_DIR = $(CURDIR)/libs

# File names
INTERPRETER_SRC = interpreter.cpp
MAIN_SRC = main.cpp
LIB_SRC = $(LIB_DIR)/main_lib.cpp

# Object files
INTERPRETER_OBJ = interpreter.o
MAIN_OBJ = main.o

# Output files
EXECUTABLE = simple
LIBRARY = $(LIB_DIR)/libmain_lib.dylib

# Build rules
all:  $(LIBRARY) $(BUILD_DIR)/$(EXECUTABLE)

# Build the library
$(LIBRARY): $(LIB_SRC)
	$(CXX) -shared -o $(LIBRARY) $(LIB_SRC) -std=c++20

# Build the executable
$(BUILD_DIR)/$(EXECUTABLE): $(BUILD_DIR)/$(INTERPRETER_OBJ) $(BUILD_DIR)/$(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(BUILD_DIR)/$(INTERPRETER_OBJ) $(BUILD_DIR)/$(MAIN_OBJ) -L$(LIB_DIR) -lmain_lib -std=c++20

# Object files
$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

.PHONY: all clean
