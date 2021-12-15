#!/usr/bin/env python3

import argparse
import sys
import struct
import math
import os


# FS constants
DISK_CAPACITY_BYTES = 1 << 22
DISK_BLK_SIZE = 4096
MAX_NAME_SIZE = 32
MAX_DATA_BLOCKS_PER_INODE = 32
INODE_SIZE = 256

# Configure this statically so it's easier
NUM_INODE_BLOCKS = 5
# 1 block is reserved for the superblock and 2 for the bitmaps
NUM_DATA_BLOCKS = DISK_CAPACITY_BYTES // DISK_BLK_SIZE - 3 - NUM_INODE_BLOCKS

MAX_INODES = NUM_INODE_BLOCKS * DISK_BLK_SIZE // INODE_SIZE

INODE_BLOCKS_OFFSET = 3 * DISK_BLK_SIZE
DATA_BLOCKS_OFFSET = INODE_BLOCKS_OFFSET + NUM_INODE_BLOCKS * DISK_BLK_SIZE

INODE_BITMAP = [0] * (DISK_BLK_SIZE // 4)
DNODE_BITMAP = [0] * (DISK_BLK_SIZE // 4)


# p64 converts a 64-bit integer to bytes
def p64(x):
    return struct.pack('<Q', x)


# p32 converts a 32-bit integer to bytes
def p32(x):
    return struct.pack('<I', x)


# p8 converts a 8-bit integer to bytes
def p8(x):
    return struct.pack('<B', x)


def write_superblock(f):
    f.seek(0, 0)

    # magic
    f.write(p64(0xbebe))

    # Number of inode blocks
    f.write(p32(NUM_INODE_BLOCKS))

    # Number of data blocks
    f.write(p32(NUM_DATA_BLOCKS))

    # First blocks that contains an inode bitmap
    f.write(p32(1))

    # Number of blocks containing inode bitmaps
    f.write(p32(1))

    # First blocks that contains a data bitmap
    f.write(p32(2))

    # Number of blocks containing data bitmaps
    f.write(p32(1))


def write_inode_header(f, inode_id, name, is_dir):
    if len(name) > MAX_NAME_SIZE:
        raise RuntimeError('Name too long: {}'.format(name))

    if inode_id * INODE_SIZE >= NUM_INODE_BLOCKS * DISK_BLK_SIZE:
        raise RuntimeError('The inode space is full. Increase the number of inode blocks.')

    # Go to the inode block
    f.seek(INODE_BLOCKS_OFFSET + inode_id * INODE_SIZE, 0)

    # Inode ID
    f.write(p32(inode_id))

    # Pad the name with null bytes
    f.write(name.encode().ljust(MAX_NAME_SIZE, b'\x00'))

    # Directory flag
    f.write(p32(int(is_dir)))


def write_file_inode(f, inode_id, name, file_size, data_block_offsets):
    write_inode_header(f, inode_id, name, False)

    assert len(data_block_offsets) <= MAX_DATA_BLOCKS_PER_INODE

    # File size
    f.write(p32(file_size))

    for dbo in data_block_offsets:
        f.write(p32(dbo))


def write_directory_inode(f, inode_id, name, children_offsets):
    write_inode_header(f, inode_id, name, True)

    assert len(children_offsets) <= MAX_DATA_BLOCKS_PER_INODE

    # Children
    f.write(p32(len(children_offsets)))

    for co in children_offsets:
        f.write(p32(co))


# reserve_inode_space finds the first free inode in the inode bitmap, marks it
# as reserved and returns its index
def reserve_inode_space():
    for i in range(MAX_INODES):
        bitmap_word_index = i // 32
        bitmap_bit_index = i % 32
        if (INODE_BITMAP[bitmap_word_index] & (1 << bitmap_bit_index)) == 0:
            INODE_BITMAP[bitmap_word_index] |= (1 << bitmap_bit_index)
            return i

    raise RuntimeError('The inode space is full. Increase the number of inode blocks.')


# write_file_data writes the data for a file to lab3FS. It returns the disk
# offsets for the file's data blocks as a list
def write_file_data(fs, data):
    num_needed_data_blocks = int(math.ceil(len(data) / DISK_BLK_SIZE))
    assert num_needed_data_blocks <= MAX_DATA_BLOCKS_PER_INODE

    block_offsets = []

    # Assume that the data blocks for a file are always contiguous because we
    # never delete
    for i in range(NUM_DATA_BLOCKS):
        bitmap_word_index = i // 32
        bitmap_bit_index = i % 32
        if (DNODE_BITMAP[bitmap_word_index] & (1 << bitmap_bit_index)) == 0:
            # We found the first free block, allocate all needed blocks
            # contiguously in the bitmap
            for j in range(num_needed_data_blocks):
                bitmap_word_index = (i + j) // 32
                bitmap_bit_index = (i + j) % 32
                assert (DNODE_BITMAP[bitmap_word_index] & (1 << bitmap_bit_index)) == 0
                DNODE_BITMAP[bitmap_word_index] |= (1 << bitmap_bit_index)

                block_offsets.append(DATA_BLOCKS_OFFSET + (i + j) * DISK_BLK_SIZE)

            # Write out the data
            fs.seek(DATA_BLOCKS_OFFSET + i * DISK_BLK_SIZE, 0)
            fs.write(data)
            return block_offsets

    raise RuntimeError('The file system is full.')


# write_file writes a file to lab3FS and returns the disk offset for its inode
def write_file(fs, name, data):
    block_offsets = write_file_data(fs, data)

    inode_id = reserve_inode_space()
    write_file_inode(fs, inode_id, name, len(data), block_offsets)
    return INODE_BLOCKS_OFFSET + INODE_SIZE * inode_id


# write_tree writes a directory from the host's filesystem and all its children
# to lab3FS. It returns the disk offset to the inode of the root
def write_tree(fs, path):
    children_offsets = []

    with os.scandir(path) as it:
        for entry in it:
            if entry.name == '.' or entry.name == '..':
                # Ignore . and ..
                continue

            if entry.is_symlink():
                print('Warning: symbolic links are not supported, ignoring {}'.format(entry.path))
            elif entry.is_file():
                with open(entry.path, 'rb') as thefile:
                    children_offsets.append(write_file(fs, entry.name, thefile.read()))
            elif entry.is_dir():
                inode_id = reserve_inode_space()

                write_directory_inode(fs, inode_id, entry.name, write_tree(fs, entry.path))
                children_offsets.append(INODE_BLOCKS_OFFSET + INODE_SIZE * inode_id)
            else:
                # What is this?
                raise RuntimeError('Unknown file type for {}'.format(entry.path))

    return children_offsets


def main():
    global NUM_INODE_BLOCKS
    parser = argparse.ArgumentParser(
        description='Build a Lab3FS image based on a directory on your computer')
    parser.add_argument('directory', type=str, help='path to the directory used to build the Lab3FS image')
    parser.add_argument('output', type=str, help='path to the output')
    parser.add_argument('--inode-blocks', type=int, default=5, help='number of inode blocks')

    args = parser.parse_args()

    NUM_INODE_BLOCKS = args.inode_blocks

    if NUM_INODE_BLOCKS <= 0:
        print('Too few inode blocks (must be at least 1)')
        return
    elif NUM_INODE_BLOCKS >= DISK_CAPACITY_BYTES // DISK_BLK_SIZE:
        print('Too many inode blocks (must be at most {})'.format(DISK_CAPACITY_BYTES // DISK_BLK_SIZE))
        return

    print('Generating Lab3FS image {} from {}... '.format(args.output, args.directory),)

    try:
        with open(args.output, 'wb') as fs:
            # Zero the entire disk
            fs.write(b'\x00' * DISK_CAPACITY_BYTES)
            write_superblock(fs)

            root_inode_id = reserve_inode_space()
            root_children_offsets = write_tree(fs, args.directory)
            write_directory_inode(fs, root_inode_id, 'root', root_children_offsets)

            # Write out the inode bitmap
            fs.seek(1 * DISK_BLK_SIZE, 0)
            for i in INODE_BITMAP:
                fs.write(p32(i))

            # And dnode bitmap
            fs.seek(2 * DISK_BLK_SIZE, 0)
            for d in DNODE_BITMAP:
                fs.write(p32(d))
    except RuntimeError as e:
        print('Error: {}'.format(e))
        return

    print('Done!')


if __name__ == '__main__':
    main()
