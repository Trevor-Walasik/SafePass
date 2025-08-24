# Makefile for SafePass - Password Manager
# Author: Trevor Walasik
# Target platform: Windows (MinGW)

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =

# Source files
SRCS = SafePass.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = SafePass.exe

# Default target
all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c files to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	del /Q *.o *.exe 2>nul || true

# For rebuilding everything
rebuild: clean all