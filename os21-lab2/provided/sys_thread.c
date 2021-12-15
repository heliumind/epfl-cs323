/**
 * @brief API implementation for system threads.
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <linux/futex.h>
#include "futex.h"
#include "sys_thread.h"
#include "scheduler_state.h"

/* The local system thread stored inside TLS */
__thread sys_thread_t* local_sys_thread = NULL;

void l2_initialize_sys_thread() {
  local_sys_thread = (sys_thread_t*) malloc(sizeof(sys_thread_t));
  assert(local_sys_thread != NULL);
  
  /* Initialize the structure */
  memset(local_sys_thread, 0, sizeof(sys_thread_t));
  local_sys_thread->sys = malloc(sizeof(thread_info_t));
  assert(local_sys_thread->sys != NULL);

  memset(local_sys_thread->sys, 0, sizeof(thread_info_t));
  local_sys_thread->sys->id = DEFAULT_TARGET;
  local_sys_thread->sys->state = SYSTHREAD;
  local_sys_thread->sys->joined_target = DEFAULT_TARGET;
  local_sys_thread->sys->yield_target = DEFAULT_TARGET;
  local_sys_thread->sys->thread_stack = malloc(sizeof(l2_stack));
}

void l2_destroy_sys_thread() {
  if (local_sys_thread == NULL) {
    return;
  }
  assert(local_sys_thread->current == NULL);
  assert(local_sys_thread->sys != NULL);
  assert(local_sys_thread->sys->thread_stack != NULL);

  free(local_sys_thread->sys->thread_stack);
  free(local_sys_thread->sys);
  free(local_sys_thread);
}

sys_thread_t* get_sys_thread() {
  return local_sys_thread;
}

void sys_thread_sleep() {
  /* Fetch the value, increment it atomically.
   * Make sure the value doesn't overflow.
   * Do the futex sleep. */
  scheduler_state_t* gstate = get_scheduler_state();
  int expected = __sync_fetch_and_add(&gstate->sleep_count, 1) + 1;
  if (expected >= gstate->sleep_count) {
    __sync_fetch_and_sub(&gstate->sleep_count, 1);
    return;
  }
  futex(&gstate->sleep_count, FUTEX_WAIT, expected);
  int res = __sync_sub_and_fetch(&gstate->sleep_count, 1);
  assert(res >= 0);
}

void sys_thread_wake_up() {
  scheduler_state_t* gstate = get_scheduler_state();
  futex(&gstate->sleep_count, FUTEX_WAKE, gstate->total_sys);
}
