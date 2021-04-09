COL_SOURCES=dep/Collectoins/sources/*.c
COL_HEADERS=dep/Collectoins/headers/*.h
SOURCES=src/*.c  
HEADERS=src/*.h 
BUILD_PATH=bin/debug/
CC=gcc
EXEC_NAME=app
CFLAGS=-Wall -Wextra -pedantic -std=c11 -g

build:
	$(CC) $(CFLAGS) $(COL_HEADERS) $(COL_SOURCES) $(HEADERS) $(SOURCES) -o $(BUILD_PATH)$(EXEC_NAME)

run:
	$(BUILD_PATH)$(EXEC_NAME)
	
