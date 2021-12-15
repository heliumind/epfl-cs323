/**
 * @file week03.c
 * @brief Outlines in C for student week 3
 *
 * @author Mark Sutherland
 */
#include <stdlib.h>
#include <string.h>
#include "schedule.h"
#include "stack.h"
#include "thread.h"
#include "thread_info.h"
#include "error.h"

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
  l1_thread_info *new_thread = (l1_thread_info *) malloc(sizeof(l1_thread_info));
  if (new_thread == NULL) // Check if thread info struct allocated correctly
    return ERRNOMEM;
  memset(new_thread, 0, sizeof(l1_thread_info));
  
  /* Initialize thread info struct */
  new_thread->id = new_tid;
  new_thread->state = RUNNABLE;
  new_thread->thread_func = start_routine;
  new_thread->thread_func_args = arg;
  
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
  /* Stack initialization */
  l1_stack *new_stack = l1_stack_new();
  if (new_stack == NULL) { // Check if new_stack allocated correctly
    free(new_thread);
    return ERRNOMEM;
  }
  // Stack alignment for System-V ABI compatibility
  l1_stack_push(new_stack, (uint64_t)0);
  l1_stack_push(new_stack, (uint64_t)l1_start);
  // Preserve rbp, r15, r14, r13, r12, rbx on new stack
  for (int i = 0; i < 6; i++)
    l1_stack_push(new_stack, (uint64_t)0);
  new_thread->thread_stack = new_stack;

  /* Week 4 initialization */
  new_thread->priority_level = TOP_PRIORITY;
  new_thread->got_scheduled = 0;
  l1_time_init(&new_thread->total_time);
  l1_time_init(&new_thread->slice_start);
  l1_time_init(&new_thread->slice_end);

  /* TODO: Add the new task for scheduling */
  add_to_scheduler(new_thread, RUNNABLE);

  /* Give the user the right thread id */
  *thread = new_tid;

  return SUCCESS;
}

l1_error l1_thread_join(l1_tid target, void **retval) {
  /* TODO: Setup necessary metadata and block yourself */
  // Get current thread to set metadata and block it
  l1_thread_info *current_thread = get_scheduler()->current;
  current_thread->state = BLOCKED;
  current_thread->joined_target = target;
  current_thread->join_recv = retval;
  current_thread->errno = SUCCESS;

  // Yield to the scheduler
  yield(-1);
  // Scheduler checks if target thread exists and sets equivalent errno

  /* Current thread re-scheduled*/ 
  l1_error resulting_errno = current_thread->errno;
  current_thread->joined_target = -1;
  current_thread->errno = SUCCESS;
  current_thread->join_recv = NULL;

  return resulting_errno;
}
