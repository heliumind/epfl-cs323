/*
 * @file fs_structs.h
 * @brief Declarations of the data structures used internally by the file system
 *
 * @author Matteo Rizzo
 */
#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* For disk_off_t */
#include "blk_io.h"

/*
 * MAX_NAME_SIZE is the maximum size in bytes of the name of a file or directory
 */
#define MAX_NAME_SIZE 32

/*
 * MAX_DATA_BLOCKS_PER_INODE is the maximum number of data blocks that an inode
 * can contain. This means that the maximum size of a file is
 * MAX_DATA_BLOCKS_PER_INODE * the size of a data block, and the maximum number
 * of children of a directory is MAX_DATA_BLOCKS_PER_INODE.
 */
#define MAX_DATA_BLOCKS_PER_INODE 32

/*
 * lab3_inode represents an inode (a data structure that holds metadata such as
 * name and size for files and folders).
 * inodes are stored on disk in special inode blocks.
 */
struct lab3_inode {
    /* id is a numeric ID that uniquely identifies the inode */
    uint32_t id;

    /* name is the name of the file or folder represented by this inode */
    char name[MAX_NAME_SIZE];

    /*
     * is_directory is true when the inode represents a directory, and false
     * when it represents a file
     */
    uint32_t is_directory;

    union {
        /* file is used when the inode represents a file */
        struct {
            /* size is the total size of the file in bytes */
            uint32_t size;

            /*
             * data_block_offsets contains the disk offsets of the blocks that
             * contain the file's data
             */
            disk_off_t data_block_offsets[MAX_DATA_BLOCKS_PER_INODE];
        } file;

        /* directory is used when the inode represents a directory */
        struct {
            /* num_children is the number of children of this directory */
            uint32_t num_children;

            /*
             * children_offsets contains the disk offsets of the inodes
             * of the children of this directory
             */
            disk_off_t children_offsets[MAX_DATA_BLOCKS_PER_INODE];
        } directory;
    };
} __attribute__((aligned(256)));

static_assert(sizeof(struct lab3_inode) == 256, "Inodes have the wrong size");

/*
 * lab3_superblock represents the superblock of a filesystem. The superblock is
 * the first block on the disk and contains metadata about the filesystem.
 */
struct lab3_superblock {
    /* magic should contain a magic nubmer specific to this filesystem */
    uint64_t magic;

    /* num_inode_blocks is the number of blocks that are allocated to inodes */
    uint32_t num_inode_blocks;

    /* num_data_blocks is the number of blocks that are allocated to data */
    uint32_t num_data_blocks;

    uint32_t first_inode_bmap;      /* First block that contains an inode bitmap */
    uint32_t num_inode_bmap_blocks; /* Number of blocks containing inode bitmaps */
    uint32_t first_dnode_bmap;      /* First block that contains a dnode bitmap */
    uint32_t num_dnode_bmap_blocks; /* Number of blocks containing dnode bitmaps */
};

/*
 * lab3_open_file represents an open file. When user opens a file the filesystem
 * creates a lab3_open_file object and stores it in memory (in the open file
 * table).
 */
struct lab3_open_file {
    /*
     * inode points to an in-memory copy of the inode associated with the open
     * file. If inode is NULL, the open file is invalid
     */
    struct lab3_inode *inode;

    /*
     * seek_offset is the offset in the file where the filesystem will read and
     * write data. seek_offset starts at 0 when a file is opened and advances
     * when the user calls read or write on the file. It can be modified with
     * lab3_seek
     */
    uint32_t seek_offset;
};
