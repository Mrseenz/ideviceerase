# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Isrc # -Wall enables all warnings, -Isrc to find headers in src/

# Linker flags - for specifying library paths.
# Add -L/your/custom/lib/path here if your libraries are not in standard system locations.
LDFLAGS = 

# Libraries to link against.
# Adjust library names/versions if compilation/linking fails (e.g., -limobiledevice-1.0 or -limobiledevice, -lusbmuxd or -lusbmuxd-2.0).
LIBS = -limobiledevice-1.0 -lusbmuxd

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
