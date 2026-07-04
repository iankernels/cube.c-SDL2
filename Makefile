# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99

# Target executable name
TARGET = cube_demo

# Source files
SRCS = main.c

# Detect OS to handle platform-specific compiler options
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    # Linux setup: Use sdl2-config tool and explicitly link the math library (-lm)
    LIBS = $(shell sdl2-config --libs) -lm
    INCLUDES = $(shell sdl2-config --cflags)
else ifeq ($(UNAME_S), Darwin)
    # macOS setup (Homebrew / Native framework)
    LIBS = $(shell sdl2-config --libs)
    INCLUDES = $(shell sdl2-config --cflags)
else
    # Windows/MinGW fallback assumptions
    LIBS = -lmingw32 -lSDL2main -lSDL2 -lm
    INCLUDES = -I/usr/include/SDL2
endif

# Default build target
all: $(TARGET)

# Link the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all clean
