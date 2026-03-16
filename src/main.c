#include <stdio.h>
#include <stdlib.h>

#include "..\include\utils.h"
#include "..\include\parse.h"

/* PNG data format
    length - 4 bytes
    type - 4 bytes
    actual data - N bytes
    CRC - 4 bytes
    -- for reference    
*/

int main(int argc, char **argv){
    
    if (argc != 3) {
        // check if it is not program name + image file
        printf("Usage: %s --command image name.png\n", argv[0]);
        return 1;
    }

    const int result = cmd_check(argv[1]);

     size_t filesize;
    uint8_t *buffer = open_file(argv[2], &filesize);

    if (!buffer) {
        perror("Buffer Error");
    }

    if (sig_check(buffer) == 1){
        perror("Not a PNG");
    }

    /*
    list of commands:
    0 - --chunk
    1 - --dump
    2 - --crc
    3 - --idhr
    4 - --plte
    */

    switch (result) {
        case 0:
            parse(buffer, filesize, result);
            break;
        case 1:
            dump(buffer, filesize);
            break;
        case 2:
            parse(buffer, filesize, result);
            break;
        case 3:
            print_idhr(buffer, filesize);
            break;
        default:
            perror("Command not found");
            break;
    }

    buffer = nullptr;
    free(buffer);
    return 0;
}