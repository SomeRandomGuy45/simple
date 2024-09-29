# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Executable name
TARGET = build/simple

# Source files and object files
SRCS = base_func.cpp bytecode.cpp helper.cpp main.cpp token.cpp vm.cpp
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = base_func.h bytecode.h config.h helper.h token.h vm.h

# Build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)