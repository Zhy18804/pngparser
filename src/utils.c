#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\utils.h"

// the list of commands
static const char *cmds[] = {"--idhr","--plte", "--crc","--chunk","--hexdump"};

// parse user command
int cmd_check(const char *cmd){
    if (!cmd) {
        return PNG_CMD_ERROR;
    }

    for (int i = 0; i < NUM_CMD; i++) {
        if (strcmp(cmd, cmds[i]) == 0) {
            return i;
        }
    }
    return PNG_CMD_ERROR;
}

int status_print(int status){
    switch (status) {
        case PNG_OK:
            break;
        case PNG_CMD_ERROR:
            printf("Command Error\n");
            break;
        case PNG_SIG_ERROR:
            printf("Signature Error\n");
            break;
        case PNG_CHUNK_ERROR:
            printf("Chunk Error\n");
            break;
        default:
            printf("Unknown Error\n");
            break;
    }
    return status;
}
uint8_t *open_file(const char *filename, size_t *filesize){
    FILE *fptr = fopen(filename,"rb");

    if (fptr == NULL){
        perror("Error reading file");
        return nullptr;
    }

    fseek(fptr, 0, SEEK_END);
    *filesize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
 
    uint8_t *buffer = malloc(*filesize);

    if (!buffer) {
        fclose(fptr);
        return nullptr;
    }
    
    fread(buffer, 1, *filesize, fptr);
    fclose(fptr);

    return buffer;
}

int sig_check(const uint8_t *buffer){
    const uint8_t sig[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    if (memcmp(buffer, sig, PNG_SIG) == 0){
        return 0;
    }
    return PNG_SIG_ERROR;
}

uint32_t reading_32to8(const uint8_t *buffer, size_t offset, size_t filesize){
    uint32_t value = 0; //0x00000000

    if (offset + 4 > filesize){
        return UINT32_MAX;
    }

    // buffer casting to 32bit int to avoid UB
    value = value | ((uint32_t)buffer[offset]) << 24;
    // moving 24 bits / 3 bytes
    // 0x12000000
    value = value | ((uint32_t)buffer[offset + 1]) << 16;
    // 0x12340000
    value = value | ((uint32_t)buffer[offset + 2]) << 8;
    // 0x12345600
    value = value | ((uint32_t)buffer[offset + 3]);
    // 0x12345678
    return value;
}

void dump(const uint8_t *buffer, size_t filesize){
    size_t offset = 0;
    size_t newline = 0;
    constexpr size_t hexperline = 32;
    constexpr size_t offset_digits = 8;
    constexpr size_t whitespace = 2;

    printf("Offset   | Data\n");
    // drawing a line
    for(size_t i = 0; i < (hexperline*3) + offset_digits + whitespace; i++) {
        printf("-");
    }
    printf("\n");

    printf("%08llu  ", offset);
    while (offset <= filesize) {
        printf("%02X ", buffer[offset]);
        newline++;
        offset++;

        if (newline == hexperline) {
            newline = 0;
            printf("\n%08llu  ", offset);
        }
    }
}