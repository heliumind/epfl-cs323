/*
 * @file fs_util.c
 * @brief Utility functions used to implement the file system
 *
 * @author Matteo Rizzo, Mark Sutherland
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs_util.h"
#include "open_file_table.h"

struct lab3_inode *find_inode_by_path(const char *path)
{
    /* TODO: Implement */
    return NULL;
}

int read_from_disk(disk_off_t disk_offset, void *buffer, size_t size)
{
    /* TODO: Implement */
    return 0;
}

/* Implementation of going to the disk, getting block 0, and returning a pointer to
 * a formatted superblock.
 */
struct lab3_superblock *get_disk_superblock(void)
{
    struct lab3_superblock *sblk = (struct lab3_superblock *)malloc(sizeof(struct lab3_superblock));

    /* Read block 0 from the disk */
    int rcode = read_from_disk(0, sblk, sizeof(struct lab3_superblock));
    if (rcode < 0) {
        free(sblk);
        return NULL;
    }

    return sblk;
}

int sanitize_fd_and_size(int fd, size_t size)
{
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        return -1;
    }

    /* Check that there is a file with this descriptor */
    if (open_file_table[fd].inode == NULL) {
        return -1;
    }

    /* size is never negative because it's unsigned */
    if (size >= MAX_DATA_BLOCKS_PER_INODE * DISK_BLK_SIZE) {
        return -1;
    }

    return 0;
}

