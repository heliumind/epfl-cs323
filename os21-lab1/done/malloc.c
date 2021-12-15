/**
 * @file malloc.c
 * @brief Implementations of custom allocators
 *
 * @author Atri Bhattacharyya, Ahmad Hazimeh
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "malloc.h"
#include "error.h"

/*********************** Standard GLIBC malloc ***********/
void *libc_malloc(size_t size) {
  return malloc(size);
}

l1_error libc_free(void *ptr) {
  free(ptr);

  return SUCCESS;
}
/**********************************************************/

/*********************** Chunk malloc *********************/

char (*l1_chunk_arena)[CHUNK_SIZE];
l1_chunk_desc_t *l1_chunk_meta;
max_align_t l1_region_magic;

void l1_chunk_init(void)
{
  /* Allocate chunk arena and metadata */
  l1_chunk_arena = malloc(CHUNK_ARENA_LENGTH * CHUNK_SIZE);
  l1_chunk_meta = malloc(CHUNK_ARENA_LENGTH * sizeof(l1_chunk_desc_t));
  /* TODO: Allocate space for metadata */ 
  if ((l1_chunk_arena == NULL) || (l1_chunk_meta == NULL)) {
    printf("Unable to allocate %d bytes for the chunk allocator\n", ALLOC8R_HEAP_SIZE);
    exit(1);
  }

  /* Generate random chunk magic */
  srand(time(NULL));
  for(unsigned i = 0; i < sizeof(max_align_t); ++i)
    *(((char *)&l1_region_magic) + i) = rand();
}

void l1_chunk_deinit(void)
{
  /* TODO: Cleanup */
  free(l1_chunk_arena);
  free(l1_chunk_meta);
}

/** Helper function:
 * Calculate amount of chunks are needed for given size in bytes
 */
size_t chunk_count(size_t bytes)
{
  return bytes / CHUNK_SIZE
         + (bytes % CHUNK_SIZE != 0 ? 1 : 0) // ceil
         + 1; // chunk for region_hdr_t
}

/** Helper function:
 * Create, initialize and copy the header chunk
 */
void set_hdr_chunk(size_t alloc_head, size_t size)
{
  l1_region_hdr_t hdr;
  hdr.magic0 = l1_region_magic;
  hdr.size = size;
  hdr.magic1 = l1_region_magic;

  memcpy((void *)l1_chunk_arena[alloc_head],
         (void *)&hdr,
         sizeof(l1_region_hdr_t));
}

/** Helper function:
 * Checks that enough chunks are free, and if not tell position of last occupied
 * chunk
 */
int verify_chunks_free(size_t start, size_t len, size_t *last_occupied)
{
  int free = 1;
  for (int i = start; i < start + len; i++) {
    if (IS_CHUNK_TAKEN(i)) {
      /* Not enough free chunks found */
      free = 0;
      *last_occupied = i;
      break;
    }
  }
  return free;
}

void *l1_chunk_malloc(size_t size)
{
  if (size == 0)
    return NULL;

  /* TODO: Implement your function here */
  /* Corner case: requested size larger than heap size */
  if (size > ALLOC8R_HEAP_SIZE) {
    l1_errno = ERRNOMEM;
    return NULL;
  }

  /* Calculate requested size in chunks */
  const size_t req_chunk_cnt = chunk_count(size);
  /* Initialize variables for search loop */
  int allocable = 0;
  size_t alloc_head = 0;
  // highest possible chunk number for request
  size_t chunk_bound = CHUNK_ARENA_LENGTH - req_chunk_cnt + 1;

  while (!allocable && alloc_head < chunk_bound) {
    if (IS_CHUNK_FREE(alloc_head)) {
      size_t last_occupied;
      if (verify_chunks_free(alloc_head, req_chunk_cnt, &last_occupied)) {
        allocable = 1;
      } else {
        /* Not enough contiguous chunks available, so jump last occupied chunk*/
        alloc_head = last_occupied + 1;
      }
    } else {
      alloc_head++;
    }
  }

  if (allocable) {
    /* Set to be allocated chunks to taken */
    for (int i = alloc_head; i < alloc_head + req_chunk_cnt; i++)
      SET_CHUNK_TAKEN(i);
    set_hdr_chunk(alloc_head, size);
    return &(l1_chunk_arena[alloc_head + 1]);
  } else {
    l1_errno = ERRNOMEM;
    return NULL;
  }
}

/** Helper function:
 * Checks magic number integrity by comparing it byte for byte
 */
int magic_equals(max_align_t *magic0, max_align_t *magic1)
{
  int equals = memcmp((void*)magic0, magic1, sizeof(max_align_t)) == 0 ? 1 : 0; 
  return equals; 
}

l1_error l1_chunk_free(void *ptr)
{
  if (ptr == NULL)
      return SUCCESS;

  /* TODO: Implement your function here */
  /* Corner case: to freed memory is outside of l1_chunk_arena */
  if (ptr < (void *)l1_chunk_arena)
    return ERRINVAL;

  /* Get header chunk */
  l1_region_hdr_t hdr;
  // Cast ptr to 
  char (*chunks)[CHUNK_SIZE] = ptr;
  char (*header_chunk)[CHUNK_SIZE] = &(chunks[-1]);
  memcpy(&hdr, header_chunk, sizeof(l1_region_hdr_t));

  /* Check if header is corrupted */
  if (magic_equals(&hdr.magic0, &l1_region_magic)
      && 
      magic_equals(&hdr.magic1, &l1_region_magic)) 
  {
    /* Free allocated chunks */
    size_t offset = (header_chunk - l1_chunk_arena);
    size_t chunk_size = chunk_count(hdr.size); // Amount of chunks to be freed
    for (size_t i = 0; i < chunk_size; i++)
      SET_CHUNK_FREE(offset + i);
    return SUCCESS;
  } else {
    return ERRINVAL;
  }
}
/**********************************************************/

/****************** Free list based malloc ****************/
l1_list_meta *l1_list_free_head = NULL;
void *l1_list_heap = NULL;
max_align_t l1_list_magic;

void l1_list_init() {
  l1_list_heap = malloc(ALLOC8R_HEAP_SIZE);
  if(l1_list_heap == NULL) {
    printf("Unable to allocate %d bytes for the list\n", ALLOC8R_HEAP_SIZE);
    exit(1);
  }

  /* Generate random list magic */
  srand(time(NULL));
  for(unsigned i = 0; i < sizeof(max_align_t); i++)
    *(((char *)&l1_list_magic) + i) = rand();

  /* TODO: Complete metadata setup */
  l1_list_meta initial_meta;
  initial_meta.capacity = ALLOC8R_HEAP_SIZE - offsetof(l1_list_meta, next);
  initial_meta.magic0 = l1_list_magic;
  initial_meta.magic1 = l1_list_magic;
  initial_meta.next = NULL;
  memcpy(l1_list_heap, &initial_meta, sizeof(l1_list_meta));
  l1_list_free_head = l1_list_heap;
}

void l1_list_deinit() {
  /* TODO: Cleanup */
  free(l1_list_heap);
}

/** Helper function:
 * Change region from free to reserved by rewiring previous next to current next
 */
void update_list(l1_list_meta *prev, l1_list_meta *cur, l1_list_meta *new_meta)
{
  if (cur == l1_list_free_head) {
    l1_list_free_head = new_meta;
  } else {
    prev->next = new_meta;
  }
}

void *l1_list_malloc(size_t req_size) {
  if(req_size == 0)
    return NULL;
  /* Corner case: requested size larger than heap size */
  if (req_size > ALLOC8R_HEAP_SIZE - offsetof(l1_list_meta, next)) {
    l1_errno = ERRNOMEM;
    return NULL;
  }

  /* TODO: Implement your function here */
  /* Calculate requested size in chunks */
  const size_t req_chunks_cnt = req_size / sizeof(max_align_t)
                              + (req_size % sizeof(max_align_t) != 0 ? 1 : 0);
                              // ceil
  const size_t aligned_sz = req_chunks_cnt * sizeof(max_align_t);
  /* Traverse through free list until a chunk is big enough */
  l1_list_meta *cur = l1_list_free_head;
  l1_list_meta *prev = NULL;
  while (cur != NULL && cur->capacity < aligned_sz) {
    prev = cur;
    cur = cur->next;
  }
  /* First-fit failed */
  if (cur == NULL)
    return NULL; 
  /** Check if free region can be split a.k.a
   * chunk is big enough to be followed by another free region 
   */
  // minimum region size
  const size_t min_reg_sz = sizeof(l1_list_meta) + sizeof(max_align_t);
  if (cur->capacity >= aligned_sz + min_reg_sz) {
    /* new meta of new free region */
    l1_list_meta free_meta;
    free_meta.capacity = cur->capacity 
                      - offsetof(l1_list_meta, next) // Metadata without next
                      - aligned_sz; // Data
    free_meta.magic0 = l1_list_magic;
    free_meta.magic1 = l1_list_magic;
    free_meta.next = cur->next;
    /* Update reserved region capacity */
    cur->capacity = aligned_sz;
    /* Find position of new meta and copy */
    l1_list_meta *free_meta_pos =
      (l1_list_meta *)(((char *)cur) + offsetof(l1_list_meta, next) + aligned_sz);
    memcpy(free_meta_pos, &free_meta, sizeof(l1_list_meta));
    update_list(prev, cur, free_meta_pos);
  } else { // Free region does not need to be split
    update_list(prev, cur, cur->next);
  }

  cur->next = NULL;
  return (void *)(((char *)cur) + offsetof(l1_list_meta, next));
}

l1_error l1_list_free(void *ptr) {
  if(ptr == NULL)
    return SUCCESS;

  /* TODO: Implement your function here */
  /* Get metadata */
  l1_list_meta *metadata_ptr = 
    (l1_list_meta *)(((char *)ptr) - offsetof(l1_list_meta, next));
  /* Check if metadata is corrupted */
  if (!magic_equals(&metadata_ptr->magic0, &l1_list_magic)
      ||
      !magic_equals(&metadata_ptr->magic1, &l1_list_magic))
  {
    return ERRINVAL;
  } 

  /* Insert region back in to free list */
  metadata_ptr->next = l1_list_free_head;
  l1_list_free_head = metadata_ptr;
  
  return SUCCESS;
}
