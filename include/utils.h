#ifndef UTILS_H
#define UTILS_H

#define PNG_SIG 8
#define SEC_LEN 4
#define NUM_CMD 5

#include <stdint.h>

// contains misc funcs

typedef enum {
    PNG_OK = 100,
    PNG_CMD_ERROR,
    PNG_SIG_ERROR,
    PNG_CHUNK_ERROR,
    PNG_BUFFER_ERROR,
} PNG_ERR;

int cmd_check(const char *cmd);

void status_print(int status);

uint8_t *open_file(const char *filename, size_t *filesize);

int sig_check(const uint8_t *buffer);

uint32_t reading_32to8(const uint8_t *buffer, size_t offset, size_t filesize);

void dump(const uint8_t *buffer, size_t filesize);

#endif 