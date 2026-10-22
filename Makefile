# Practica 2 - Complejidad Computacional 2026/27
# Simulador determinista de maquinas de Turing multicinta.

CXX ?= g++
CPPFLAGS ?= -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -O2
LDFLAGS ?=

BUILD_DIR := build
OBJECT_DIR := $(BUILD_DIR)/obj
APP := $(BUILD_DIR)/tm_simulator
TEST_APP := $(BUILD_DIR)/test_runner

LIB_SOURCES := src/Transition.cpp src/MachineDefinition.cpp src/ConfigurationParser.cpp src/Tape.cpp src/TuringMachine.cpp
LIB_OBJECTS := $(patsubst src/%.cpp,$(OBJECT_DIR)/%.o,$(LIB_SOURCES))
MAIN_OBJECT := $(OBJECT_DIR)/main.o
TEST_OBJECT := $(OBJECT_DIR)/test_turing_machine.o

.PHONY: all clean test

all: $(APP)

$(APP): $(LIB_OBJECTS) $(MAIN_OBJECT)
	$(CXX) $(LDFLAGS) $^ -o $@

$(TEST_APP): $(LIB_OBJECTS) $(TEST_OBJECT)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJECT_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJECT_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJECT): tests/test_turing_machine.cpp
	@mkdir -p $(OBJECT_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test: $(TEST_APP) $(APP)
	./$(TEST_APP)
	python3 tests/test_cli.py

clean:
	rm -rf $(OBJECT_DIR) $(APP) $(TEST_APP)
