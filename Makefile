CXX = clang++
CXXFLAGS = -std=c++11 -Wall -g -Iinclude

SRC_DIR = src
TEST_DIR = tests

SOURCES = $(SRC_DIR)/door_states.cpp \
          $(SRC_DIR)/door_controller.cpp \
          $(SRC_DIR)/motor_driver.cpp \
          $(SRC_DIR)/motor_pair.cpp \
          $(SRC_DIR)/button_driver.cpp \
          $(SRC_DIR)/automatic_door_system.cpp \
          $(SRC_DIR)/mock_hardware.cpp

MAIN    = $(SRC_DIR)/main.cpp
TEST    = $(TEST_DIR)/tests.cpp

OBJECTS  = $(SOURCES:.cpp=.o)
MAIN_OBJ = $(MAIN:.cpp=.o)
TEST_OBJ = $(TEST:.cpp=.o)

TARGET = door_app

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(OBJECTS) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o test_runner $(OBJECTS) $(TEST_OBJ)
	./test_runner
	rm -f $(TEST_OBJ) test_runner

clean:
	rm -f $(OBJECTS) $(MAIN_OBJ) $(TEST_OBJ) $(TARGET) test_runner

.PHONY: all run test clean
