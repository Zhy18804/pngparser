#include <stdint.h>
#include "..\include\utils.h"
#include "..\include\parse.h"

void crc_table(uint32_t crctable[]) {
    for (int i = 0; i < 256; i++){
        uint8_t b = i;
        uint32_t remainder = b;

        // long division
        for (uint8_t bit = 8; bit > 0; bit--) {
            // checking if LSB is 1
            if (remainder & 1) {
                uint32_t POLYNOMIAL = 0xEDB88320;
                // POLYNOMIAL = 11101101 10111000 10000011 00100000
                remainder = (remainder >> 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder >> 1); 
            }
        }
        crctable[(size_t)b] = remainder;
    }
}

uint32_t update_crc(uint32_t crc,  const uint8_t *buffer, size_t length, const uint32_t crctable[]) {
    uint32_t newcrc = crc;

    for(size_t i = 0; i < length; i++) {
        newcrc = crctable[(newcrc ^ buffer[i]) & 0xff] ^ (newcrc >> 8);
    }

    return newcrc;
}

int verify_crc(const pngchunk *chunk) {
    uint32_t crc = 0xfffffffful;

    uint32_t crctable[256];
    crc_table(crctable);

    crc = update_crc(crc, (const uint8_t *)chunk->type, SEC_LEN, crctable);
    crc = update_crc(crc, chunk->data, chunk->length, crctable);
    crc ^= 0xffffffffu;

    if (crc == chunk->crc) {
        return PNG_OK;
    }
    return PNG_CHUNK_ERROR;
}

void print_crc(const pngchunk *chunk, int status){
    if (status == PNG_OK) {
        printf("%s CRC OK\n", chunk->type);
    } else {
        printf("%s CRC ERROR\n", chunk->type);
    }
}