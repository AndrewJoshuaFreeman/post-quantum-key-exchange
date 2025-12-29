CXX := g++
CXXFLAGS := -std=c++17 -O3 -ffast-math -Wall -Wextra -pedantic
INCLUDES := -Iinclude

SRC_DIR := src
OBJ_DIR := obj

TARGET := pqkex

SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/params.cpp \
        $(SRC_DIR)/modq.cpp \
        $(SRC_DIR)/prng.cpp \
        $(SRC_DIR)/matrix.cpp \
        $(SRC_DIR)/sha256.cpp \
        $(SRC_DIR)/lwe_kex.cpp \
        $(SRC_DIR)/bench.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ_DIR)

