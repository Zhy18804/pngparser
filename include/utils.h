#ifndef UTILS_H
#define UTILS_H

#define PNG_SIG 8
#define SEC_LEN 4
#define NUM_CMD 4

#include <stdint.h>

// contains misc funcs

int cmd_check(const char *cmd);

uint8_t *open_file(const char *filename, size_t *filesize);

uint32_t reading_32to8(const uint8_t *buffer, size_t offset, size_t filesize);

int sig_check(const uint8_t *buffer);

#endif 