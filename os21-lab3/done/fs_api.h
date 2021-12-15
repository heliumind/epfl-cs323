/*
 * @file fs_api.h
 * @brief Functions declarations and constants for the file system API.
 *
 * @author Matteo Rizzo, Mark Sutherland
 */
#pragma once

#include <stdint.h>

#include "blk_io.h"

/*
 * Purpose: opens a file and returns a file descriptor for that file.
 *          lab3_open fails if
 *           - path is NULL
 *           - path points to a directory,
 *           - path is relative
 *           - the file doesn't exist
 *           - the file is already open.
 *
 * Arguments:
 *  path - the path of the file to lab3_open
 *
 * Returns:
 *  - a file descriptor for the newly opened file on success
 *  - -1 on error
 */
int lab3_open(const char *path);

/*
 * Purpose: closes an open file. lab3_close fails if the file descriptor is
 *          invalid.
 *
 * Arguments:
 *  fd - the file descriptor to close
 *
 * Returns:
 *  0 on success and -1 on failure
 */
int lab3_close(int fd);

/*
 * Purpose: reads at most size bytes from an open file into buffer. lab3_read
 *          may read less than size bytes if it reaches the end of the file.
 *          lab3_read updates the seek offset of the file on success. lab3_read
 *          fails if the file descriptor, buffer, or size are invalid.
 *
 * Arguments:
 *  fd - an open descriptor for a file that contains the data to read
 *  buffer - a pointer to a buffer in memory where the data will be stored
 *  size - the maximum number of bytes to read into buffer
 *
 * Returns: the number of bytes read from the file, or -1 if there was an error.
 *          The seek offset of the file is only modified if there was no error.
 */
int32_t lab3_read(int fd, void *buffer, uint32_t size);

/*
 * Purpose: modifies the seek offset of a file. lab3_seek fails if the file
 *          descriptor or offset are invalid
 *
 * Arguments:
 *  fd - an open descriptor for the file whose seek offset will be changed
 *  offset - the new seek offset
 *
 * Returns: 0 on success, -1 on error
 */
int lab3_seek(int fd, uint32_t offset);

/*
 * Purpose: lists the contents of a directory. This function returns the names
 *          of the directory's children as an array of strings in out, and the
 *          size of the array in out_size. lab3_readdir fails if:
 *           - path, out, or out_size are NULL
 *           - path is relative
 *           - path points to a file
 *           - the directory does not exist
 *
 * Arguments:
 *  path - the path to the directory to list
 *  out - lab3_readdir will write the address of the array of names here
 *  out_size - lab3_readdir will write the size of the array of names here
 *
 * Returns: 0 on success, -1 on error
 */
int lab3_readdir(const char *path, char ***out, uint32_t *out_size);

