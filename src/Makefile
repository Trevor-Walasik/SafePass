# Makefile for SafePass - Password Manager
# Author: Trevor Walasik
# Target platform: Windows (MinGW)

CC = gcc
CFLAGS = -Wall -Wextra -g

# Default target
all: SafePass.exe

# Link the object files into the final executable
SafePass.exe: SafePass.o aes.o
	$(CC) $(CFLAGS) SafePass.o aes.o -o SafePass.exe

SafePass.o: SafePass.c SafePassStructDefs.h
	$(CC) $(CFLAGS) -c SafePass.c

aes.o: aes.c aes.h
	$(CC) $(CFLAGS) -c aes.c

aes.h:

SafePassStructDefs.h:

SafePass.h:

# Clean up build files
clean:
	del /Q *.o *.exe *.obj *.bak *~ 2>nul || true
