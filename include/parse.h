#ifndef CHUNK_H
#define CHUNK_H

#include <stdio.h>
#include <stdint.h>

// contains funcs to parse

typedef struct {
    uint32_t length;
    char type[5];
    uint8_t* data;
    uint32_t crc;
} pngchunk;

void get_color_type(uint8_t color_type, char **color_type_interpretation);

void print_idhr(const uint8_t *buffer, size_t filesize);

void dump(const uint8_t *buffer, size_t filesize);

int read_chunk(const uint8_t *buffer, size_t *offset, size_t filesize, pngchunk *chunk);

void print_chunk(pngchunk *chunk);

void parse(const uint8_t *buffer, size_t filesize, int cmd);

#endif