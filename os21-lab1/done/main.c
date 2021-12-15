/**
 * @file main.c
 * @brief Multithreading example
 *
 * @author Mark Sutherland
 */
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include "schedule.h"
#include "sched_policy.h"
#include "thread.h"
#include "thread_info.h"

/* Setting the allocator interface to libc. 
 * We will implement custom allocators in week 5 
 * See `main` for more details.
 * IMPORTANT: Use l1_malloc/l1_free in created threads
 * */
void *(*l1_malloc)(size_t) = libc_malloc;
l1_error (*l1_free)(void *) = libc_free;
void (*l1_init)(void) = NULL;
void (*l1_deinit)(void) = NULL;

/* is_bar allocates and returns a bool.
 * The function checks if the argument is the string "bar"
 */
void *is_bar(void *arg) {
  char *str = (char *)arg;

  bool *check = (bool *) l1_malloc(sizeof(bool));
  if(check == NULL)
    return NULL;
  *check = (strcmp(str, "bar") == 0 ? true : false);

  return check;
}

void* foo(void* arg) {
  l1_tid other_thread, another_thread;
  bool *other_check, *another_check;

  /* Creating two threads, with different arguments 
   * Then joining on them in reverse order */
  if((l1_thread_create(&other_thread, is_bar, "not_bar") != SUCCESS) ||
     (l1_thread_create(&another_thread, is_bar, "bar") != SUCCESS) ||
     (l1_thread_join(another_thread, (void **)&another_check) != SUCCESS) || 
     (l1_thread_join(other_thread, (void **)&other_check) != SUCCESS))
    return NULL;

  printf("The checks returned: %d %d\n", *other_check, *another_check);

  l1_free(other_check);
  l1_free(another_check);

  return NULL;
}

int main(int argc, char **argv)
{
  /* Before week 5: 
   * We will work on a custom memory allocator in week 5.
   * Before week 5, ignore the allocator init and deinit.
   * However, all threads which you create should use 
   * l1_malloc and l1_free for memory allocation/free.
   * For week 5:
   * There will be a call to l1_init to initialize the 
   * memory allocator. This should allocate space for the 
   * heap and setup the necessary metadata.
   */
  if(l1_init != NULL)
    l1_init();

  /* Call to setup the scheduler */
  initialize_scheduler(l1_mlfq_policy);
  /* Creating a thread. A unique identifier for this thread 
   * will be stored in `tid`. The thread will run the function
   * `foo` with a NULL argument. */
  l1_tid tid;
  l1_thread_create(&tid, foo, (void*)("baz")); 

  /* Finally, we start the scheduler. This function will return
   * only when all threads have finished executing */
  schedule(); 

  /* See comments at beginning of main.
   * For week 5:
   * This call is used to clean up the heap space allocated
   * at the beginning of main. 
   */
  if(l1_deinit != NULL)
    l1_deinit();

  return 0;
}
