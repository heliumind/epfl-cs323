/**
 * @file thread_list.c
 * @bried Implementation of thread_list_t declarations.
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include  <stdlib.h>
#include "thread_list.h"


void thread_list_init(thread_list_t* list) {
  assert(list != NULL);
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

thread_info_t* thread_list_remove(thread_list_t* list, thread_info_t* thread) {
  if (!list || !thread) {
    return NULL;
  }
  thread_info_t* prev = thread->prev;
  thread_info_t* next = thread->next;
  /* We are the only ones in the list. */
  if (!prev && !next) {
     list->head = list->tail = NULL;
     goto rm_prolog;
  }
  if (!prev) {
    list->head = next;
    next->prev = NULL;
    goto rm_prolog;
  }
  if (!next) {
    list->tail = prev; 
    prev->next = NULL;
    goto rm_prolog;
  }
  prev->next = next;
  next->prev = prev;
rm_prolog:
  list->size--;
  thread->prev = thread->next = NULL;
  return thread;
}


thread_info_t* thread_list_find(thread_list_t* list, tid_t tid) {
  for (thread_info_t* cur = list->head; cur != NULL; cur = cur->next) {
    if (cur->id == tid) {
      return cur;
    }
  }
  return NULL; 
}

thread_info_t* thread_list_rotate(thread_list_t* list) {
  if (thread_list_is_empty(list)) {
    return NULL;
  }
  thread_info_t* elem = thread_list_pop(list);
  thread_list_add(list, elem);
  return elem;
}


thread_info_t* thread_list_pop(thread_list_t* list) {
  if (list == NULL) {
    return NULL;
  }
  return thread_list_remove(list, list->head);
}

void thread_list_add(thread_list_t* list, thread_info_t* thread) {
  if (list == NULL || thread == NULL) {
    return; 
  }
  assert(thread->prev == NULL && thread->next == NULL);
  if (thread_list_is_empty(list)) {
    list->head = list->tail = thread;
    goto add_prolog;
  }

  list->tail->next = thread;
  thread->prev = list->tail;
  list->tail = thread;
add_prolog:
  thread->next = NULL;
  list->size++;
}

bool thread_list_is_empty(thread_list_t* list) {
  if (list == NULL) {
    return false;
  }
  return list->size == 0;
}



