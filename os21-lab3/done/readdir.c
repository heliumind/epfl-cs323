/*
 * @file readdir.c
 * @brief Implementation of readdir
 *
 * @author Matteo Rizzo
 */
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs_api.h"
#include "fs_util.h"

int lab3_readdir(const char *path, char ***out, uint32_t *out_size)
{
    /* TODO: Implement */
    /* Sanity checks */
    if (path == NULL || out == NULL || out_size == NULL || path[0] != '/')
        return -1;
    /* Get inode of directory */
    struct lab3_inode *dir = find_inode_by_path(path);
    /* Check if directory exists */
    if (dir == NULL) {
        free(dir);
        return -1;
    }
    /* Check if returned inode is really a directory */
    if (dir->is_directory == 0) {
        free(dir);
        return -1;
    }
    uint32_t num_children = dir->directory.num_children;
    *out_size = num_children;
    *out = (char **)malloc(num_children * sizeof(char *));
    /* Iterate over children and fill names */
    struct lab3_inode *child = (struct lab3_inode *)malloc(sizeof(struct lab3_inode));
    for (size_t i = 0; i < num_children; i++) {
        read_from_disk(dir->directory.children_offsets[i], child, sizeof(struct lab3_inode));
         (*out)[i] = (char *)malloc(MAX_NAME_SIZE);
         memcpy((*out)[i], child->name, MAX_NAME_SIZE);
    }
    free(child);
    free(dir);
    return 0;
}
