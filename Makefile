TARGET = build/dvd_overlay
LIBS = -lopengl32 -lglfw3
CC = gcc
CFLAGS = -std=c99

default: program
all: default

OBJECTS = $(wildcard src/*.c)
HEADERS = $(wildcard lib/*.h)
STATIC_ASSETS = $(wildcard assets/*.o)

program:
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(STATIC_ASSETS) -I include/ -I lib/ $(LIBS)
