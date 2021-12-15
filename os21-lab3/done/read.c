/*
 * @file read.c
 * @brief Implementation of read
 *
 * @author Matteo Rizzo, Mark Sutherland
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs_api.h"
#include "fs_util.h"
#include "open_file_table.h"

int32_t lab3_read(int fd, void *buffer, uint32_t size)
{
    /* TODO: Implement */
    /* Sanity checks */
    if (buffer == NULL) // buffer invalid
        return -1;
    /* Check if file descriptr invalid */
    if (fd < 0 || fd >= MAX_OPEN_FILES)
        return -1;
    if (open_file_table[fd].inode == NULL)
        return -1;    
    /* Get fd inode and seek_offset */
    struct lab3_inode *inode = open_file_table[fd].inode;
    uint32_t seek_offset = open_file_table[fd].seek_offset;
    /* Check if size is invalid */
    if (seek_offset + size > inode->file.size) {
        return -1;
    }
    /* Calculate start_addr of read request */
    disk_off_t dblknum = seek_offset & ~DISK_BLKNUM_MASK;
    disk_off_t start_addr = inode->file.data_block_offsets[dblknum] + (seek_offset & DISK_BLKNUM_MASK);
    /* Read data */
    uint32_t bytes_read;
    uint32_t b_to_read;
    for (bytes_read = 0; bytes_read < size; bytes_read += b_to_read) {
        int flag = 0; // if request crosses block boundary
        uint32_t b_left = size - bytes_read;
        /* Request crosses a boundary */
        if (b_left + (seek_offset & DISK_BLKNUM_MASK) > DISK_BLK_SIZE) {
            b_to_read = DISK_BLK_SIZE - (seek_offset & DISK_BLKNUM_MASK);
            flag = 1;
        } else {
            b_to_read = b_left;
        }
        /* Check if EOF reached */
        if (open_file_table[fd].seek_offset-1 + bytes_read + b_to_read > inode->file.size) {
            printf("About to reach EOF!\n");
            b_to_read = inode->file.size - bytes_read - open_file_table[fd].seek_offset;
            read_from_disk(start_addr, (char*)buffer + bytes_read, b_to_read);
            bytes_read += b_to_read;
            break;
        }
        assert((seek_offset & DISK_BLKNUM_MASK) + b_to_read <= DISK_BLK_SIZE);
        read_from_disk(start_addr, (char*)buffer + bytes_read, b_to_read);
        if (flag) {
            seek_offset = 0;
            dblknum++;
            start_addr = inode->file.data_block_offsets[dblknum];
        }
    }
    open_file_table[fd].seek_offset = (open_file_table[fd].seek_offset + bytes_read) % inode->file.size;
    return bytes_read;
}
