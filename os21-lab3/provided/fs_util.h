/*
 * @file fs_util.h
 * @brief Declarations of utility functions used to implement the file system
 *
 * @author Matteo Rizzo
 */
#pragma once

#include "fs_structs.h"

/*
 * Purpose: find_inode_by_path takes a path and returns an in-memory copy of
 * the inode for the file or directory that is located at that path on the
 * filesystem.
 *
 * Arguments:
 *  path - the path to the file or directory to find
 *
 * Returns: a pointer to an in-memory copy of the inode on success, NULL on
 *          error. The returned inode is allocated with malloc(), and it's the
 *          responsibility of the caller to free it.
 */
struct lab3_inode *find_inode_by_path(const char *path);

/*
 * Purpose: read_from_disk reads size bytes from disk into buffer. read_from_disk
 *          fails if the disk offset/size are out of range, if it fails to read
 *          the requested data from disk, or if the request crosses a block
 *          boundary (i.e. if the data that the caller requested is not
 *          completely contained in one disk block)
 *
 * Arguments:
 *  disk_offset - the disk offset to read from.
 *  buffer - the buffer to read the data into. This doesn't need to be a multiple
 *           of the block size
 *  size - how many bytes to read. This doens't need to be a multiple of the block
 *         size
 *
 * Returns: 0 on success, -1 on error.
 */
int read_from_disk(disk_off_t disk_offset, void *buffer, size_t size);

/*
 * Purpose: Go to the disk and get the superblock from it.
 *
 * Arguments:
 *  None
 *
 * Returns:
 *  a pointer which contains a copy of the disk superblock.
 *  If the read from disk failed, the return value is NULL.
 *  It is the caller's responsibility to free this pointer as it was allocated
 *  with malloc()
 */
struct lab3_superblock *get_disk_superblock(void);

/*
 * Purpose: Sanitize an fd and size parameter for a read/write call.
 *
 * Arguments:
 *  fd_to_verify - the fd to check for validity
 *  size_to_verify - the size to check for validity
 *
 * Returns:
 *  0 if both are valid
 *  -1 if either or both is invalid
 */
int sanitize_fd_and_size(int fd, size_t size);

