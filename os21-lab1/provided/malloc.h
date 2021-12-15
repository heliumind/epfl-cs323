/**
 * @file malloc.h
 * @brief Header files for custom allocator
 *
 * @author Atri Bhattacharyya, Ahmad Hazimeh
 */
#pragma once
#include <stddef.h>
#include <stdlib.h>
#include "error.h"

#define ALLOC8R_HEAP_SIZE (1024 * 1024)

/* Week 5: Interface for heap memory allocator */
extern void *(*l1_malloc)(size_t);
extern l1_error (*l1_free)(void *);
extern void (*l1_init)(void);
extern void (*l1_deinit)(void);

/****** Standard libc based allocator *******************/
void *libc_malloc(size_t size);
l1_error libc_free(void *ptr);

/****** Chunk allocator: l1_chunk ******/
/* The chunk allocator is a simple bin allocator with one type of bins. Starting
 * with a fixed-size heap region, it divides the region into fixed-size chunks.
 * The collection of all available chunks is henceforth called the "arena". The
 * arena has `CHUNK_ARENA_LENGTH` consecutive chunks, each `CHUNK_SIZE` bytes
 * long, starting at `l1_chunk_arena`. The chunk allocator also maintains, for
 * each chunk, a data structure of type `l1_chunk_desc_t` which describes the
 * status of that chunk. These data structures are stored in an array starting
 * at `l1_chunk_meta`. This array is mainly used to check whether a chunk is
 * allocated or free.
 * 
 * To allocate a region of some size S, a contiguous sequence of chunks is
 * reserved to fit the requested size, and an additional chunk directly
 * preceding that region must be allocated to store metadata describing that
 * region. For instance, with a chunk size of 4KiB, a requested region size of
 * 10KiB, and starting with an empty arena, one possible allocation output could
 * be:
 *   - Chunk 0: region metadata
 *   - Chunk 1: data chunk
 *   - Chunk 2: data chunk
 *   - Chunk 3: data chunk (partially used, but entirely allocated)
 * 
 * The chunk storing region metadata accesses a data structure of type
 * `l1_region_hdr_t`, stored at the beginning of that chunk.
 */

#define CHUNK_SIZE (1 << 12) // 4KiB 
#define CHUNK_ARENA_LENGTH (ALLOC8R_HEAP_SIZE / CHUNK_SIZE)

#define IS_CHUNK_FREE(x) (l1_chunk_meta[x] == 0)
#define IS_CHUNK_TAKEN(x) (l1_chunk_meta[x] == 1)
#define SET_CHUNK_FREE(x) (l1_chunk_meta[x] = 0)
#define SET_CHUNK_TAKEN(x) (l1_chunk_meta[x] = 1)

/**
 * The data structure used to store the metadata for each allocated chunk.
 */
typedef char l1_chunk_desc_t;

/**
 * The data structure used to store the metadata for each allocated region. It
 * is stored in a header chunk directly preceding the region.
 * 
 * Every allocated region of contiguous chunks is preceded by a chunk solely
 * dedicated to store metadata about that region. This metadata includes the
 * size of the region and a couple magic values surrounding the size field, used
 * as a sanity check when freeing the region.
 */
typedef struct {
  max_align_t magic0;
  size_t size;
  max_align_t magic1;
} l1_region_hdr_t;

/**
 * A pointer to the start of the chunk arena. This is expected to be allocated
 * during initialization.
 */
extern char (*l1_chunk_arena)[CHUNK_SIZE];

/**
 * A pointer to an array of chunk metadata structures. This is expected to be
 * allocated during initialization.
 */
extern l1_chunk_desc_t *l1_chunk_meta;

/**
 * A random magic value used for verifying that the region header is valid.
 */
extern max_align_t l1_region_magic;

/**
 * @brief      Initializes the chunk arena and metadata
 * 
 * Allocates the entire chunk arena, consisting of `CHUNK_ARENA_LENGTH` chunks,
 * each `CHUNK_SIZE` bytes long. Additionally, it allocates the chunk metadata
 * storage region, `l1_chunk_meta`.
 * 
 * If this function fails to allocate any of the required memory areas, it must
 *  exit with a status code of 1.
 * 
 */
void l1_chunk_init(void);

/**
 * @brief      Releases the chunk arena and metadata.
 * 
 * Releases the allocated chunk arena and metadata memory areas.
 * 
 * This function must not fail.
 * 
 */
void l1_chunk_deinit(void);

/**
 * @brief      Allocates a region of chunks
 * 
 * Searches in the arena for a contiguous sequence of chunks to store the
 * requested size, in addition to a chunk directly preceding that region, which
 * serves as a region header.
 * 
 * If the requested size is 0, the function must return a NULL pointer.
 * 
 * If the function fails to find a suitable region in the arena, it must set
 * `l1_errno` to ERRNOMEM and return a NULL pointer.
 *
 * @param[in]  size  The size, in bytes, of the region to be allocated.
 *
 * @return     A pointer to the first byte of the first allocated data chunk, or
 *             NULL if it fails.
 */
void *l1_chunk_malloc(size_t size);

/**
 * @brief      Releases a region of chunks
 *
 * Returns all chunks in the provided region back to a "free" state. The
 * function must first verify that the provided pointer lies on valid chunk
 * boundaries, and that the region header chunk has valid magic values.
 * 
 * If the provided pointer is NULL, the function must return SUCCESS.
 * 
 * If the function fails to verify the validity of the provided pointer, it must
 * return ERRINVAL.
 *
 * @param      ptr   The pointer to the first byte in the region to be freed.
 *
 * @return     SUCCESS if no errors occured. Otherwise, ERRINVAL.
 */
l1_error l1_chunk_free(void *ptr);

/****** Meta data for the free list allocator: l1_list *******/
void *l1_list_malloc(size_t);
l1_error l1_list_free(void *);
void l1_list_init(void);
void l1_list_deinit(void);

typedef struct l1_list_meta {
  /* Every allocated region is preceded by a region containing the
   * metadata for that region. The magics serve two purposes:
   * 1. Approx method for checking if a pointer passed to free is
   *    a valid one.
   * 2. If the beginning of this struct is aligned to `sizeof(max_align_t)`,
   *    the beginning of `next` will also be similarly aligned. */
  max_align_t magic0;
  size_t capacity;
  max_align_t magic1 __attribute__((aligned(sizeof(max_align_t))));
  /* When a region of memory is allocated, it will span the 
   * address start from &next, and span `capacity` bytes. 
   * When a region of memory is free, the next few bytes will
   * contain a pointer to the next region of free memory */
  struct l1_list_meta *next;
} l1_list_meta;

extern l1_list_meta *l1_list_free_head;
extern void *l1_list_heap;
extern max_align_t l1_list_magic;

