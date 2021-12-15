/*
 * @file open.c
 * @brief Implementation of open
 *
 * @author Matteo Rizzo
 */
#include <stdlib.h>

#include "fs_api.h"
#include "fs_util.h"
#include "open_file_table.h"

int lab3_open(const char *path)
{
    /* TODO: implement */
    /* Check corner cases */
    if (path == NULL // path is NULL
        || path[0] != '/') // path is relative 
        return -1;
    struct lab3_inode *inode = find_inode_by_path(path);
    if (inode == NULL // file doesn't exist
        || inode->is_directory == 1) { // path points to a directory
        free(inode);
        return -1;
    }
    /* Iterate over open file table to find a free slot */
    int idx = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_file_table[i].inode == NULL) {
            idx = i;
        } else {
            /* Check if file is already opened */
            if (open_file_table[i].inode->id == inode->id) {
                free(inode);
                return -1;
            }
        }
    }
    /* No free spot in open file table is found */
    if (idx == -1) {
        free(inode);
        return -1;
    }
    open_file_table[idx].inode = inode;
    open_file_table[idx].seek_offset = 0;
    return idx;
}
