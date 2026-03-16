#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include "..\include\parse.h"

// contains funcs related to PNG CRC

void make_crc_table(uint32_t crctable[]);

uint32_t update_crc(uint32_t crc,  const uint8_t *buffer, size_t length, const uint32_t crctable[]);

int verify_crc(const pngchunk *chunk);

void print_crc(const pngchunk *chunk, int f);

#endif