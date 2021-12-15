/*
 * @file blk_io.c
 * @brief Implementations for emulated disk API.
 *
 * @author Mark Sutherland, Matteo Rizzo
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "blk_io.h"
#include "disk.h"

int check_offset_valid(disk_off_t off)
{
    /* Validate that the offset is less than disk capacity */
    if (off >= DISK_CAPACITY_BYTES) {
        return -1;
    }
    /* Validate offset is a multiple of the disk block size */
    disk_off_t bsize_mask = (DISK_BLK_SIZE - 1);
    if (off & bsize_mask) {
        return -1;
    }
    return 0;
}

int open_emu_disk(const char *fname)
{
    int fd = open(fname, O_SYNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        int errno_retd = errno;
        fprintf(stderr,
            "open_emu_disk failed with errno %d when calling open(2)"
            "with name %s\n",
            errno_retd, fname);
        return -1;
    }

    /* Allocate the global disk struct. */
    if (the_disk != NULL) {
        fprintf(stderr, "open_emu_disk failed because the global disk struct "
                        "is already allocated.\n");
        return -1;
    }

    the_disk = (struct emu_disk_t *)malloc(sizeof(struct emu_disk_t));
    the_disk->fd = fd;

    /* Copy the fname into the struct. */
    the_disk->emu_fname = strdup(fname);
    if (the_disk->emu_fname == NULL) {
        unsigned errno_strdup = errno;
        fprintf(stderr, "open_emu_disk failed because the call to strdup()"
                        "returned NULL, with errno %u.\n",errno_strdup);
        return -1;
    }

    return 0;
}

void close_emu_disk(void)
{
    close(the_disk->fd);
    free(the_disk->emu_fname);
    free(the_disk);
    the_disk = NULL;
}

int get_block(disk_off_t off, void *usr_buf)
{
    int off_valid = check_offset_valid(off);
    if (off_valid < 0) {
        return -2;
    }

    /* Read from the underlying emu disk file at offset off */
    ssize_t bytes_read = pread(the_disk->fd, usr_buf, DISK_BLK_SIZE, off);
    if (bytes_read < 0) {
        int errno_retd = errno;
        fprintf(stderr,
            "get_block failed with errno %d when calling pread() to "
            "offset: %u\n",
            errno_retd, off);
        return -1;
    } else if (bytes_read == 0) {
        fprintf(stderr,
            "get_block failed to read anything from the underlying disk. "
            "EOF error. Check the offset (%d)\n",
            off);
        return -1;
    }
    assert(bytes_read == DISK_BLK_SIZE);
    return 0;
}

