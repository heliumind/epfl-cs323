/*
 * @file open_file_table.h
 * @brief Declaration of the global open file table
 *
 * @author Matteo Rizzo
 */
#pragma once

/* MAX_OPEN_FILES is the maximum number of files that can be open at once */
#define MAX_OPEN_FILES 32

#include "fs_structs.h"

/*
 * open_file_table contains a lab3_open_file structure for each open file.
 * File descriptors are indices into this table
 */
extern struct lab3_open_file open_file_table[MAX_OPEN_FILES];
