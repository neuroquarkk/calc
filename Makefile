PROJECT = calc

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE = -Iinclude

CC = gcc
DEBUG_FLAGS = -g -Wall -DDEBUG
RELEASE_FLAGS = -O2 -Wall
LDFLAGS = -lm

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/$(PROJECT)

all: release

release: CFLAGS = $(RELEASE_FLAGS)
release: prep $(TARGET)

debug: CFLAGS = $(DEBUG_FLAGS)
debug: .prep $(TARGET)

.prep:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

run:
	$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

distclean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

help:
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@echo "  make           Build in release mode (default)"
	@echo "  make release   Build with optimizations (-O2)"
	@echo "  make debug     Build with debug flags (-g -DDEBUG)"
	@echo "  make run       Run the compiled binary (bin/calc)"
	@echo "  make clean     Remove only object files (build/)"
	@echo "  make distclean Remove all generated files (build/ and bin/)"
	@echo "  make help      Show this help message"

.PHONY: all release clean distclean .prep run help
