/**
 * @brief implementation of mutex functions.
 *
 * @author Adrien Ghosn
 */

#include <assert.h>
#include "utils.h"
#include "mutex.h"
#include "sys_thread.h"

void mutex_init(mutex_t* m) {
  /* TODO: Implement */
  m->owner = DEFAULT_TARGET;
  // Init spinlock
  spinlock_t lock;
  spinlock_init(&lock);
  m->lock = lock;
  // Init blocked list
  thread_list_t list;
  thread_list_init(&list);
  m->blocked = list;
}

void mutex_lock(mutex_t* m) {
  /* TODO: Implement */
  tid_t tid = get_current_thread()->id;
  /* Fastpath */
  spinlock_lock(&m->lock);
  if(__sync_bool_compare_and_swap(&m->owner, DEFAULT_TARGET, tid)) {
    spinlock_unlock(&m->lock);
    return; 
  }
  /* Mutex is held by someone else */
  // Add yourself to blocked list
  thread_list_add(&m->blocked, get_current_thread());
  // Block yourself and release spinlock
  cond_wait((void*)&m->lock, SPINLOCK);
  spinlock_lock(&m->lock);
  if(__sync_bool_compare_and_swap(&m->owner, tid, tid)) {
    spinlock_unlock(&m->lock);
    return;
  }
}

void mutex_unlock(mutex_t* m) {
  /* TODO: Implement */
  tid_t tid = get_current_thread()->id;
  spinlock_lock(&m->lock);
  assert(m->owner == tid);

  if (thread_list_is_empty(&m->blocked)) {
    __sync_val_compare_and_swap(&m->owner, tid, DEFAULT_TARGET);
    spinlock_unlock(&m->lock);
  } else { // There are other threads waiting for this mutex, wake one up
    thread_info_t *next_thread = thread_list_pop(&m->blocked);
    __sync_val_compare_and_swap(&m->owner, tid, next_thread->id);
    spinlock_unlock(&m->lock);
    tsafe_unblock_thread(next_thread);
  }
}


