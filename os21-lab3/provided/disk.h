/*
 * @file disk.h
 * @brief Declaration of the global disk pointer
 *
 * @author Mark Sutherland
 */
#pragma once
#include <stdint.h>

/* Defines the number of bytes in the emulated disk.
 */
#define DISK_CAPACITY_BYTES (1 << 22)

/* Defines the granularity of a disk I/O.
 */
#define DISK_BLK_SHIFT 12
#define DISK_BLK_SIZE (1 << DISK_BLK_SHIFT)

/* Convenience macros */
#define NUM_DISK_BLKS_RAW (DISK_CAPACITY_BYTES >> DISK_BLK_SHIFT)
#define DISK_BLKNUM_MASK (DISK_BLK_SIZE - 1)

/* Defines the type for a disk offset which is passed
 * to all functions in this file.
 */
typedef uint32_t disk_off_t;

/* Struct for tracking the disk's underlying file,
 * internal state, etc
 */
struct emu_disk_t {
    int fd;          /* The fd of the underlying disk file. */
    char *emu_fname; /* The name of the underlying disk file. */
};

extern struct emu_disk_t *the_disk;
