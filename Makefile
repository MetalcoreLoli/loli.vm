SOURCES=src/*.c
BUILD_PATH=bin/debug/
CC=gcc
EXEC_NAME=app
CFLAGS=-Wall -Wextra -pedantic -std=c11

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(BUILD_PATH)$(EXEC_NAME)

run:
	$(BUILD_PATH)$(EXEC_NAME)
	
