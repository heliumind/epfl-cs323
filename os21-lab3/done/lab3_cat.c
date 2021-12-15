#include <stdio.h>

#include "blk_io.h"
#include "fs_api.h"

/* Purpose: prints the contents of a file to the standard output, like the
 *          system utility cat.
 *
 * Arguments:
 *  - path: the path to the file to print.
 *
 * Returns:
 *  - 0 on success and -1 on error.
 */
static int lab3_cat(const char *path)
{
    int fd = lab3_open(path);
    if (fd < 0) {
        return -1;
    }

    char buf[4096] = {0};
    int32_t read_ret;

    do {
        read_ret = lab3_read(fd, buf, sizeof(buf));
        if (read_ret < 0) {
            lab3_close(fd);
            return -1;
        }

        fwrite(buf, 1, read_ret, stdout);
    } while (read_ret == sizeof(buf));

    lab3_close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <disk image> <path to file>\n", argv[0]);
        return 0;
    }

    if (open_emu_disk(argv[1]) < 0) {
        return -1;
    }

    if (lab3_cat(argv[2]) < 0) {
        fprintf(stderr, "Failed to print the contents of %s\n", argv[2]);
        close_emu_disk();
        return -1;
    }

    close_emu_disk();

    return 0;
}
