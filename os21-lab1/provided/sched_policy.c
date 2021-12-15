/**
 * @file sched_policy.c
 * @brief Implementations for custom schedulers
 *
 * @author Atri Bhattacharyya, Mark Sutherland
 */
#include <stdio.h>
#include <stdlib.h>
#include "sched_policy.h"
#include "schedule.h"

/** Round robin just returns the oldest thread in RUNNABLE state */
l1_thread_info* l1_round_robin_policy(l1_thread_info* prev, l1_thread_info* next) {
  if (next != NULL) {
    return next;
  }
  l1_scheduler_info *scheduler = get_scheduler();
  return thread_list_rotate(&scheduler->thread_arrays[RUNNABLE]);
}

/** Schedules the thread with the smallest amount of cycles so far */
l1_thread_info* l1_smallest_cycles_policy(l1_thread_info* prev, l1_thread_info* next) {

  /* TODO: Your code */

  return next; 
}

/** Schedules threads according to mlfq policy */
l1_thread_info* l1_mlfq_policy(l1_thread_info* prev, l1_thread_info* next) {

  /* TODO: Your code */

  return next; 
}

