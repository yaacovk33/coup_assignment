# yaaccovkrawiec@gmail.com

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I./include -I./tests
SRCDIR = src
TESTDIR = tests
OBJDIR = obj

# Source files
SOURCES = $(SRCDIR)/Player.cpp $(SRCDIR)/Role.cpp $(SRCDIR)/Game.cpp
DEMO_SRC = $(SRCDIR)/Demo.cpp
TEST_SRC = $(TESTDIR)/Test.cpp
GUI_SRC = $(SRCDIR)/GUI.cpp

# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
DEMO_OBJ = $(OBJDIR)/Demo.o
TEST_OBJ = $(OBJDIR)/Test.o
GUI_OBJ = $(OBJDIR)/GUI.o

# Executables
DEMO_EXEC = coup_demo
TEST_EXEC = coup_test
GUI_EXEC = coup_gui

# SFML libraries (adjust path if needed)
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Default target
all: $(DEMO_EXEC) $(TEST_EXEC)

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/Test.o: $(TESTDIR)/Test.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build demo executable
$(DEMO_EXEC): $(OBJECTS) $(DEMO_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build test executable
$(TEST_EXEC): $(OBJECTS) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Run demo
Main: $(DEMO_EXEC)
	./$(DEMO_EXEC)

# Run tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Run with valgrind
valgrind: $(DEMO_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(DEMO_EXEC)

# Build GUI executable
$(GUI_EXEC): $(OBJECTS) $(GUI_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

# Run GUI
gui: $(GUI_EXEC)
	./$(GUI_EXEC)

# Clean build files
clean:
	rm -rf $(OBJDIR) $(DEMO_EXEC) $(TEST_EXEC) $(GUI_EXEC) $(CONSOLE_EXEC)

# Console UI (no SFML required)
CONSOLE_OBJ = $(OBJDIR)/ConsoleUI.o
CONSOLE_EXEC = coup_console

$(CONSOLE_EXEC): $(OBJECTS) $(CONSOLE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Run console UI
console: $(CONSOLE_EXEC)
	./$(CONSOLE_EXEC)

# Phony targets
.PHONY: all Main test valgrind gui console clean