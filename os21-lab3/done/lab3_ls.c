#include <stdio.h>
#include <stdlib.h>

#include "blk_io.h"
#include "fs_api.h"

/* Purpose: prints the contents of a directory to the standard output, like the
 *          system utility ls.
 *
 * Arguments:
 *  - path: the path to the directory to list.
 *
 * Returns:
 *  - 0 on success and -1 on error.
 */
static int lab3_ls(const char *path)
{
    char **out;
    uint32_t out_size;

    if (lab3_readdir(path, &out, &out_size) < 0) {
        return -1;
    }

    for (uint32_t i = 0; i < out_size; i++) {
        puts(out[i]);
        free(out[i]);
    }

    free(out);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <disk image> <path to directory>\n", argv[0]);
        return 0;
    }

    if (open_emu_disk(argv[1]) < 0) {
        return -1;
    }

    if (lab3_ls(argv[2]) < 0) {
        fprintf(stderr, "Failed to list the contents of %s\n", argv[2]);
        close_emu_disk();
        return -1;
    }

    close_emu_disk();

    return 0;
}
