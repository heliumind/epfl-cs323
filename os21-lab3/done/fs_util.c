/*
 * @file fs_util.c
 * @brief Utility functions used to implement the file system
 *
 * @author Matteo Rizzo, Mark Sutherland
 */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "disk.h"
#include "fs_util.h"
#include "open_file_table.h"

struct lab3_inode *find_inode_by_path(const char *path)
{
    /* TODO: Implement */
    struct lab3_inode *prev = (struct lab3_inode *)malloc(sizeof(struct lab3_inode));
    struct lab3_inode *cur = (struct lab3_inode *)malloc(sizeof(struct lab3_inode));
    if (prev == NULL || cur == NULL) {
        return NULL;
    }
    /* Get root directory inode */
    struct lab3_superblock *sblk = get_disk_superblock();
    disk_off_t root_offset = (sblk->first_dnode_bmap + sblk->num_dnode_bmap_blocks) * DISK_BLK_SIZE;
    read_from_disk(root_offset, prev, sizeof(struct lab3_inode));
    free(sblk);
    /* Find first component of path */
    char *npath = strdup(path);
    char *token = strtok(npath, "/");
    while (token != NULL) {
        /* Check if token is directory */
        if (prev->is_directory == 1) {
            /* Iterate through children */
            int found = 0;
            for (int i = 0; i < prev->directory.num_children; i++) {
                read_from_disk(prev->directory.children_offsets[i], cur, sizeof(struct lab3_inode));
                if (strcmp(token, cur->name) == 0) {
                    found = 1;
                    memcpy(prev, cur, sizeof(struct lab3_inode));
                    break;
                }
            }
            if (found == 0) {
                free(npath);
                free(prev);
                free(cur);
                fprintf(stderr, "Requested path does not exist!\n");
                return NULL;
            }
        }
        else { // still tokens to look for but prev is not a directory
            fprintf(stderr, "Invalid path!\n");
            free(prev);
            free(npath);
            free(cur);
            return NULL;
        }
        token = strtok(NULL, "/");
    }
    free(npath);
    free(cur);
    free(token);
    return prev;
}

int read_from_disk(disk_off_t disk_offset, void *buffer, size_t size)
{
    /* TODO: Implement */
    if (buffer == NULL)
        return -1;
    if (disk_offset + size > DISK_CAPACITY_BYTES) {
        fprintf(stderr, "Trying to read data past the end of disk\n");
        return -1;
    }
    /* Compute block number address and block offset that contains data */
    disk_off_t blk_addr = disk_offset & ~DISK_BLKNUM_MASK;
    assert(disk_offset >= blk_addr);
    disk_off_t blk_offset = disk_offset & DISK_BLKNUM_MASK;
    // Check if request crosses a block boundary
    if (blk_offset + size > DISK_BLK_SIZE) {
        fprintf(stderr, "Trying to read data beyond a requested block\n");
        return -1;
    }
    uint8_t blk_buf[DISK_BLK_SIZE];
    /* Read block from disk */
    int rcode = get_block(blk_addr, blk_buf);
    if (rcode < 0) {
        fprintf(stderr, "get_block failed with errno %d\n", errno);
        return -1;
    }
    memcpy(buffer, blk_buf + blk_offset, size);
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

