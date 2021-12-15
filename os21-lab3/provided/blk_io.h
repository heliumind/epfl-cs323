/*
 * @file blk_io.h
 * @brief Declarations for emulated disk API.
 *
 * @author Mark Sutherland, Matteo Rizzo
 */
#pragma once
#include <stdint.h>

#include "disk.h"

/* Purpose: Open an already existing file
 * with the specific name. This will be the emulated
 * disk on which the FS will be written.
 *
 * Arguments:
 *  fname - the name of the file which emulates the disk
 *
 * Returns:
 *  0 if the disk struct was created successfully
 *  -1 if the disk struct creation failed
 */
int open_emu_disk(const char *fname);

/* Purpose: Close the emulated disk without deleting
 * the underlying file.
 *
 * Arguments:
 *  none
 *
 * Returns:
 *  none
 */
void close_emu_disk(void);

/* Purpose: Close the emulated disk and delete the underlying file.
 *
 * Arguments:
 *  none
 *
 * Returns:
 *  none
 */
void delete_emu_disk(void);

/* Purpose: Reads the underlying disk and returns the
 * data which is written there.
 * Arguments:
 *  off - the offset to read the disk at
 *  usr_buf - pointer to a buffer at which to write the
 *  data which comes back from the disk
 *
 * Returns:
 *  -2 if the offset is invalid
 *  -1 if the underlying disk system failed with an error
 *  0 otherwise on success
 */
int get_block(disk_off_t off, void *usr_buf);

