# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Isrc # -Wall enables all warnings, -Isrc to find headers in src/

# Linker flags - for specifying library paths
LDFLAGS = # e.g., -L/usr/local/lib or -L/path/to/libimobiledevice/lib

# Libraries to link against
LIBS = # e.g., -limobiledevice-1.0 -lusbmuxd # Placeholder for libimobiledevice and libusbmuxd

# Name of the executable
TARGET = ideviceerase

# Source files and object files
SRCS = src/ideviceerase.c
OBJS = $(SRCS:.c=.o) # Replaces .c with .o, so src/ideviceerase.c becomes src/ideviceerase.o

# Default target: builds the executable
all: $(TARGET)

# Target to link the executable
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "$(TARGET) built successfully."

# Target to compile C source files into object files
# This is a pattern rule that applies to any .o file that depends on a .c file in src/
src/%.o: src/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean target: removes build artifacts
clean:
	@echo "Cleaning up build artifacts..."
	rm -f $(TARGET) $(OBJS)
	@echo "Cleanup complete."

# Phony targets: targets that don't represent actual files
.PHONY: all clean
