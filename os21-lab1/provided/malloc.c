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
}

void *l1_chunk_malloc(size_t size)
{
  if (size == 0)
    return NULL;

  /* TODO: Implement your function here */
}

l1_error l1_chunk_free(void *ptr)
{
  if (ptr == NULL)
      return SUCCESS;

  /* TODO: Implement your function here */
  return SUCCESS;
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
}

void l1_list_deinit() {
  /* TODO: Cleanup */
}

void *l1_list_malloc(size_t req_size) {
  if(req_size == 0)
    return NULL;

  /* TODO: Implement your function here */
  return NULL;
}

l1_error l1_list_free(void *ptr) {
  if(ptr == NULL)
    return SUCCESS;

  /* TODO: Implement your function here */
  return SUCCESS;
}
