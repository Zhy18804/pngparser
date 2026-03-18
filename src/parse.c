#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\include\crc.h"
#include "..\include\utils.h"
#include "..\include\parse.h"

void get_color_type(uint8_t color_type, char **color_type_interpretation) {
    switch (color_type) {
        case 0:
            *color_type_interpretation = "(Grayscale)";
            break;
        case 2:
            *color_type_interpretation = "(RGB Triple)";
            break;
        case 3:
            *color_type_interpretation = "(PLTE Index)";
            break;
        case 4:
            *color_type_interpretation = "(Grayscale Sample, Alpha Sample)";
            break;
        case 6:
            *color_type_interpretation = "(RGB triple, alpha Sample)";
            break;
        default:
            *color_type_interpretation = "(Unknown Type)";
            break;
    }
}

void print_idhr(const pngchunk *chunk){
    size_t idhr_offset = 0;

    uint32_t width = reading_32to8(chunk->data, idhr_offset, 4);
    idhr_offset += 4;

    uint32_t height = reading_32to8(chunk->data, idhr_offset, idhr_offset + 4);
    idhr_offset += 4;
            
    uint8_t bit_depth = chunk->data[idhr_offset];
    idhr_offset++;
            
    uint8_t color_type = chunk->data[idhr_offset];
    idhr_offset++;
            
    uint8_t compression = chunk->data[idhr_offset];
    idhr_offset++;
            
    uint8_t filter = chunk->data[idhr_offset];
    idhr_offset++;
            
    uint8_t interlace = chunk->data[idhr_offset];

    char* color_type_interpretation;
    get_color_type(color_type, &color_type_interpretation);

    printf("size        = %u x %u\n", width, height);
    printf("bit_depth   = %u\n", bit_depth);
    printf("color_type  = %u, %s\n", color_type, color_type_interpretation);
    printf("compression = %u\n", compression);
    printf("filter      = %u\n", filter);
    printf("interlace   = %u\n", interlace);
}

int print_plte(const pngchunk *chunk){
    if (chunk->length % 3 != 0) {
        return PNG_CHUNK_ERROR;
    }
    constexpr size_t line = 34;

    printf("Offset | Data\n");
    // drawing a line
    for(size_t i = 0; i < line; i++) {
        printf("-");
    }
    printf("\n");

    size_t offset = 0;
    while (offset < chunk->length) {
        printf("%04llu    ", offset);
        printf("Red:%03i ", chunk->data[offset]);
        printf("Blue:%03i ", chunk->data[offset + 1]);
        printf("Green:%03i\n", chunk->data[offset + 2]);
        offset += 3;
    }
    return PNG_OK;
}

void print_chunk(pngchunk *chunk) {
    printf("Type: %s  ", chunk->type);

    if (chunk->type[0] >= 'A' && chunk->type[0] <= 'Z') {
        printf("Critical,  ");
    } else {
        printf("Ancillary, " );
    }

    if (chunk->type[1] >= 'A' && chunk->type[1] <= 'Z') {
        printf("Public, ");
    } else {
        printf("Private, " );
    }

    if (chunk->type[2] >= 'A' && chunk->type[2] <= 'Z') {
        printf("Unsafe-to-copy ");
    } else {
        printf("Safe-to-copy " );
    }

    printf("Length = %u\n", chunk->length);
}

int read_chunk(const uint8_t *buffer, size_t *offset, size_t filesize, pngchunk *chunk) {

    if (*offset + SEC_LEN + SEC_LEN > filesize) {
        return -1;
    }

    // get length
    chunk->length = reading_32to8(buffer, *offset, filesize);
    *offset += SEC_LEN;

    if (chunk->length == UINT32_MAX){
        return PNG_CHUNK_ERROR;
    }

    //copy type
    memcpy(chunk->type, buffer + *offset, SEC_LEN);
    chunk->type[4] = '\0';
    *offset += SEC_LEN;

    if (*offset + chunk->length + SEC_LEN > filesize) {
        return PNG_CHUNK_ERROR;
    }

    if (chunk->type[2] >= 'a' && chunk->type[2] <= 'z') {
        return PNG_CHUNK_ERROR;
    }

    //copy data
    if (chunk->length != 0) {
        chunk->data = malloc(chunk->length);
        memcpy(chunk->data, buffer + *offset, chunk->length);

        *offset += chunk->length;
    } else {
        chunk->data = nullptr;
    }

    //copy crc
    chunk->crc = reading_32to8(buffer, *offset, filesize);
    *offset += SEC_LEN;

    if (chunk->crc == UINT32_MAX) {
        return PNG_CHUNK_ERROR;
    }

    return PNG_OK;
}

int parse(const uint8_t *buffer,  size_t filesize, int cmd){

    size_t offset = PNG_SIG;
    // offset starts at 8
    
    while (offset + 12 <= filesize){
        pngchunk chunk;

        // all chunk values stored in struct
        if (read_chunk(buffer, &offset, filesize, &chunk) != PNG_OK) {
            return PNG_CHUNK_ERROR;
        }

        if (cmd == 0 && strncmp(chunk.type, "IHDR", 4) == 0) {
            print_idhr(&chunk);
        }

        if (cmd == 1 && strncmp(chunk.type, "PLTE", 4) == 0) {
            print_plte(&chunk);
        }

        if (cmd == 2) {
            int status = verify_crc(&chunk);
            print_crc(&chunk, status);
        }

        if (cmd == 3){
            print_chunk(&chunk);
        }
    }
    return PNG_OK;
}