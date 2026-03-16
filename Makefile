CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ = build/main.o build/utils.o build/parse.o build/crc.o
SRC = src/main.c src/utils.c src/parse.c src/crc.c
HDR = include/utils.h include/parse.h include/crc.h

TARGET = pngparser

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rmdir /s /q build
	del /q $(TARGET).exe