/*
 * @file seek.c
 * @brief Implementation of seek
 *
 * @author Matteo Rizzo
 */
#include "fs_api.h"
#include "open_file_table.h"

int lab3_seek(int fd, uint32_t offset)
{
    /* TODO: Implement */
    if (fd < 0 || fd >= MAX_OPEN_FILES)
        return -1;
    if (open_file_table[fd].inode == NULL)
        return -1;
    if (offset >= open_file_table[fd].inode->file.size)
        return -1;
    open_file_table[fd].seek_offset = offset;
    return 0;
}
