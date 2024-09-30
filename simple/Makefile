# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Linker flags (OpenGL, GLFW, and others)
#LDFLAGS = -lglfw3 -lopengl32 -lgdi32 -lstdc++fs  # Use POSIX-style path

# Executable name
TARGET = build/simple

# Source files and object files
SRCS = base_func.cpp bytecode.cpp helper.cpp main.cpp token.cpp vm.cpp #glad.c
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = base_func.h bytecode.h config.h helper.h token.h vm.h #glad.h

# Build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)