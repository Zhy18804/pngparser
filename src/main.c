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
    int status = PNG_OK;

    if (argc != 3) {
        // check if it is not program name + image file
        printf("Usage: %s --command image name.png\n", argv[0]);
        status = PNG_CMD_ERROR;
        return status_print(status);
    }

    const int result = cmd_check(argv[1]);

    if (result == PNG_CMD_ERROR) {
        status = result;
        return status_print(status);
    }

    size_t filesize;
    uint8_t *buffer = open_file(argv[2], &filesize);

    if (!buffer) {
        status = PNG_BUFFER_ERROR;
        return status_print(status);
    }

    if (sig_check(buffer) == 1){
        status = PNG_BUFFER_ERROR;
        return status_print(status);
    }

    /*
    list of commands:
    0 - --idhr
    1 - --plte
    2 - --crc
    3 - --chunk
    4 - --hexdump
    */

    if (result <= 3) {
        status = parse(buffer, filesize, result);
    } else {
        dump(buffer, filesize);
    }

    buffer = nullptr;
    free(buffer);
    return status_print(status);
}