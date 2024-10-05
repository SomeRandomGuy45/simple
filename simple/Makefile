# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -fvisibility=default

# Target names
LIB_TARGET_BASE = build/libsimple
MAIN_TARGET = build/simple

# Source files and object files
SRCS = base_func.cpp bytecode.cpp helper.cpp token.cpp vm.cpp
OBJS = $(SRCS:.cpp=.o)
MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    LIB_EXT = dll
    SHARED_FLAG = -shared
    LINK_FLAG = -Lbuild -lsimple
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIB_EXT = dylib
        SHARED_FLAG = -dynamiclib
        LINK_FLAG = -Lbuild -lsimple
    else
        LIB_EXT = so
        SHARED_FLAG = -shared
        LINK_FLAG = -Lbuild -lsimple
    endif
endif

# Full library target with extension
LIB_TARGET = $(LIB_TARGET_BASE).$(LIB_EXT)

# Target to create the dynamic library
create_dll: $(LIB_TARGET)

# Build the library target
$(LIB_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) -o $(LIB_TARGET) $(OBJS)

# Target to create the main executable (depends on the library being built first)
create_main: $(MAIN_TARGET)

# Build the main executable, linking the library (depends on MAIN_OBJ and LIB_TARGET)
$(MAIN_TARGET): $(MAIN_OBJ) $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) -o $(MAIN_TARGET) $(MAIN_OBJ) $(LINK_FLAG)

# Compile main object file after building the library
$(MAIN_OBJ): $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

# Compile object files for the library
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build
clean:
	rm -f $(OBJS) $(MAIN_OBJ) $(LIB_TARGET_BASE).dll $(LIB_TARGET_BASE).dylib $(LIB_TARGET_BASE).so $(MAIN_TARGET)

# Run the program
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)
