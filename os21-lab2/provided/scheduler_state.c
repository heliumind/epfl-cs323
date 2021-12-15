/**
 * @brief Implementation of global state functions.
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "scheduler_state.h"

scheduler_state_t* global_state = NULL;

void initialize_scheduler_state(sched_policy policy, int nb_sys_threads) {
  assert(nb_sys_threads > 0);
  assert(nb_sys_threads < IS_OVER);
  global_state = (scheduler_state_t*)malloc(sizeof(scheduler_state_t));
  assert(global_state != NULL);

  /*Initialize state*/
  memset(global_state, 0, sizeof(scheduler_state_t));
  global_state->total_sys = nb_sys_threads;
  global_state->select_next = policy;
  for (int i = 0; i < NUM_THREAD_STATES; i++) {
    pthread_mutex_init(&global_state->list_locks[i], NULL); 
  }

  pthread_mutex_init(&global_state->exists_lock, NULL);
  global_state->exists.head = NULL;
  global_state->exists.tail = NULL;
}

void destroy_scheduler_state() {
  if (global_state != NULL) {
    free(global_state);
    global_state = NULL;
  }
}

scheduler_state_t* get_scheduler_state() {
  return global_state;
}

tid_t get_uniq_tid() {
  return __sync_add_and_fetch(&global_state->next_tid, 1);
}

thread_info_t* select_next(thread_info_t* cur, thread_info_t* target) {
  thread_info_t* next = global_state->select_next(cur, target);
  /* Verify it has been removed from the list & is runnable */
  assert(next->prev == NULL && next->next == NULL && next->state == RUNNABLE);
  return next;
}

void register_thread(thread_info_t* t) {
  assert(t->prev == NULL && t->next == NULL);
  pthread_mutex_lock(&global_state->exists_lock);
  thread_list_t* list = &global_state->exists;
  if(thread_list_is_empty(list)) {
    list->head = list->tail = t;
    goto register_prolog;
  }
  list->tail->ex_next = t;
  t->ex_prev = list->tail;
  list->tail = t;
register_prolog:
  t->ex_next = NULL;
  list->size++;
  pthread_mutex_unlock(&global_state->exists_lock);
}

thread_info_t* unregister_thread(thread_info_t* t) {
  pthread_mutex_lock(&global_state->exists_lock);
  thread_list_t* list = &global_state->exists;
  if (!list || !t) {
    return NULL;
  }
  thread_info_t* prev = t->ex_prev;
  thread_info_t* next = t->ex_next;
  /* we are the only ones in the list. */
  if (!prev && !next) {
     list->head = list->tail = NULL;
     goto rm_prolog;
  }
  if (!prev) {
    list->head = next;
    next->ex_prev = NULL;
    goto rm_prolog;
  }
  if (!next) {
    list->tail = prev; 
    prev->ex_next = NULL;
    goto rm_prolog;
  }
  prev->ex_next = next;
  next->ex_prev = prev;
rm_prolog:
  list->size--;
  t->ex_prev = t->ex_next = NULL;
  pthread_mutex_unlock(&global_state->exists_lock);
  return t;
}

thread_info_t* does_thread_exists(tid_t tid) {
  thread_list_t* list = &global_state->exists;
  for (thread_info_t* cur = list->head; cur != NULL; cur = cur->ex_next) {
    if (cur->id == tid) {
      return cur;
    }
  }
  return NULL;
}

void tsafe_enqueue_thread(thread_info_t* t, thread_state_t s) {
  assert(t->prev == NULL && t->next == NULL && t->state == s);
  pthread_mutex_lock(&global_state->list_locks[s]);
  thread_list_add(&global_state->thread_arrays[s], t);
  pthread_mutex_unlock(&global_state->list_locks[s]);
}

void remove_thread(thread_info_t* t, thread_state_t s) {
  assert(t->prev != NULL && t->next != NULL && t->state == s); 
  pthread_mutex_lock(&global_state->list_locks[s]);
  thread_list_remove(&global_state->thread_arrays[s], t);
  pthread_mutex_unlock(&global_state->list_locks[s]);
}

void lock_list(thread_state_t s) {
  pthread_mutex_lock(&global_state->list_locks[s]);
}

void unlock_list(thread_state_t s) {
  pthread_mutex_unlock(&global_state->list_locks[s]);
}

thread_info_t* tsafe_find_and_remove_thread(tid_t target, thread_state_t s) {
  thread_info_t* result = NULL;
  pthread_mutex_lock(&global_state->list_locks[s]);
  result = thread_list_find(&global_state->thread_arrays[s], target);
  if (result != NULL) {
    thread_list_remove(&global_state->thread_arrays[s], result);
  }
  pthread_mutex_unlock(&global_state->list_locks[s]);
  return result;
}

void lock_zombie_joining() {
  pthread_mutex_lock(&global_state->list_locks[ZOMBIE]);
  pthread_mutex_lock(&global_state->list_locks[JOINING]);
}

void unlock_zombie_joining() {
  pthread_mutex_unlock(&global_state->list_locks[JOINING]);
  pthread_mutex_unlock(&global_state->list_locks[ZOMBIE]);
}

void lock_all_lists() {
  lock_zombie_joining();
  pthread_mutex_lock(&global_state->list_locks[RUNNABLE]);
  pthread_mutex_lock(&global_state->exists_lock);
}

void unlock_all_lists() {
  pthread_mutex_unlock(&global_state->exists_lock);
  pthread_mutex_unlock(&global_state->list_locks[RUNNABLE]);
  unlock_zombie_joining();
}
