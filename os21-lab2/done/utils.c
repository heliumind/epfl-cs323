/**
 * @brief Implements helper functions for students. 
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include "thread.h"
#include "utils.h"
#include "sys_thread.h"
#include "scheduler_state.h"

void clean_up_global_state() {
  destroy_scheduler_state();
}

void initialize_and_launch(sched_policy policy, int sys_threads, thread_func_t your_main, void* args) {
  clean_up_global_state();
  initialize_scheduler_state(policy, sys_threads);
  tid_t main_tid; 
  l2_thread_create(&main_tid, your_main, args);
  init_sys_and_launch();
  clean_up_global_state();
}

void cond_wait(void* lock, lock_type_t tpe) {
  assert(lock != NULL);
  assert(tpe == SPINLOCK || tpe == MUTEX);
  sys_thread_t* sys = get_sys_thread();
  assert(sys != NULL);
  thread_info_t* current = sys->current;
  assert(current != NULL);
  current->to_release = lock;
  current->lock_type = tpe;
  current->state = BLOCKED;
  yield(-1);
  assert(current->state == RUNNING);
  assert(current->to_release == NULL);
  assert(current->lock_type == NONE);
}

void tsafe_unblock_thread(thread_info_t* t){
  assert(t != NULL);
  assert(t->state == BLOCKED);
  assert(t->prev == NULL && t->next == NULL);
  t->state = RUNNABLE;
  tsafe_enqueue_thread(t, RUNNABLE);
}

thread_info_t* get_current_thread() {
  sys_thread_t* sys = get_sys_thread();
  assert(sys != NULL);
  assert(sys->current != NULL);
  return sys->current;
}
