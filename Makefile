# ============================================================================
# Makefile for Net-MIDI project
# 
# Build system for compiling and linking all modules
# ============================================================================

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = -lpcap

# Source files
SOURCES = main.cpp network_capture.cpp file_parser.cpp midi_builder.cpp user_interface.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = net-midi

# Default target
all: $(EXECUTABLE)

# Link executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful! Run with: ./$(EXECUTABLE)"

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -f *.o
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Display help
help:
	@echo "Available targets:"
	@echo "  make       - Build the project"
	@echo "  make clean - Remove build artifacts"
	@echo "  make rebuild - Clean and build"
	@echo "  make help  - Show this message"

.PHONY: all clean rebuild help
