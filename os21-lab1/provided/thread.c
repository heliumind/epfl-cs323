/**
 * @file week03.c
 * @brief Outlines in C for student week 3
 *
 * @author Mark Sutherland
 */
#include <stdlib.h>
#include "schedule.h"
#include "stack.h"
#include "thread.h"
#include "thread_info.h"

void l1_start(void) {
  l1_thread_info* cur = get_scheduler()->current;
  /* enter execution */
  void* ret = cur->thread_func(cur->thread_func_args); 
  cur->retval = ret; 
  cur->state = ZOMBIE;
  /* Let the scheduler do the cleanup */
  yield(-1); 
}

l1_error l1_thread_create(l1_tid *thread, void *(*start_routine)(void *), void *arg) {
  l1_tid new_tid = get_uniq_tid();
  /* TODO: Allocate l1_thread_info struct for new thread,
   * allocate stack for the thread. */
  ;
  /* TODO: Setup stack for new task. At the bottom of the stack is a fake stack 
   * frame for l1_start, as described in the handout. This will allow the 
   * new thread to return to l1_start after it finished working. 
   * Hint: To figure out which registers need to be saved on the stack, and the 
   * order in which they need to be stored, explore the state of the stack in 
   * `switch_asm`. Also, what are the default values required by `l1_start`.
   * Binaries compiled by `gcc` use the System-V ABI which defines which registers
   * are saved by a function's caller and callee. 
   * See https://wiki.osdev.org/System_V_ABI
   * In `switch_asm`, we only save the registers are not already saved.
   */
  ;
  /* TODO: Add the new task for scheduling */
  ;

  /* Give the user the right thread id */
  *thread = new_tid;

  return SUCCESS;
}

l1_error l1_thread_join(l1_tid target, void **retval) {
  /* TODO: Setup necessary metadata and block yourself */
  return SUCCESS;
}
