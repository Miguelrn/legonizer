# ===============================
# Pixeler - Professional Makefile
# Windows + Git Bash / MSYS2
# ===============================

# Compiler
CC = gcc

# Project name
TARGET = pixeler

# Folders
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = lib

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Dependency files
DEP = $(OBJ:.o=.d)

# Flags
CFLAGS = -Wall -Wextra -std=c11 -I$(INC_DIR)
DEBUGFLAGS = -g
RELEASEFLAGS = -O2
LDFLAGS = -L$(LIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm

# ===============================
# Default target
# ===============================
all: debug

# ===============================
# Debug build
# ===============================
debug: CFLAGS += $(DEBUGFLAGS)
debug: $(BIN_DIR)/$(TARGET)

# ===============================
# Release build
# ===============================
release: CFLAGS += $(RELEASEFLAGS)
release: $(BIN_DIR)/$(TARGET)

# ===============================
# Link executable
# ===============================
$(BIN_DIR)/$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# ===============================
# Compile source files
# ===============================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# ===============================
# Run
# ===============================
run: debug
	./$(BIN_DIR)/$(TARGET)

# ===============================
# Debugger
# ===============================
gdb: debug
	gdb $(BIN_DIR)/$(TARGET)

# ===============================
# Clean
# ===============================
clean:
	rm -rf $(BUILD_DIR)

# ===============================
# Rebuild
# ===============================
rebuild: clean all

# ===============================
# Include dependency files
# ===============================
-include $(DEP)

# ===============================
# Phony targets
# ===============================
.PHONY: all debug release run gdb clean rebuild