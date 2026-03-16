#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\utils.h"

// the list of commands
static const char *cmds[] = {"--chunk","--hexdump", "--crc","--idhr","--plte", "--text", "--gama", "--phys"};

/*
might wanna try to concatenate IDAT chunks
then use zlib to decompress them
then png filters
maybe export with ppm format
*/

// takes in the command string then returns an int 
int cmd_check(const char *cmd){
    if (!cmd) {
        return -1;
    }

    for (int i = 0; i < NUM_CMD; i++) {
        if (strcmp(cmd, cmds[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// opens file of png, extracts the data and modifies filesize arg
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
    return 1;
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