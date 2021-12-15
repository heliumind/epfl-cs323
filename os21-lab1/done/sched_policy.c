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
  if (next != NULL) {
  return next; 
  }
  l1_thread_info *smallest_cycles = NULL;
  l1_thread_list *list = &get_scheduler()->thread_arrays[RUNNABLE];
  l1_time min_time = __LONG_MAX__;
  // Traverse through RUNNABLE list
  for (l1_thread_info* cur = list->head; cur != NULL; cur = cur->next) {
    if (l1_time_is_smaller(cur->total_time, min_time)) {
      min_time = cur->total_time;
      smallest_cycles = cur;
    }
  }
  return smallest_cycles;
}

/** Helper function: 
 * Increase priority of threads that did not run at their current priority 
 */
void mlfq_boost(l1_thread_list *list) {
  if (list == NULL)
    return;
  // Traverse through list and increase priority of each thread
  for (l1_thread_info* cur = list->head; cur != NULL; cur = cur->next) {
    if (!cur->got_scheduled) {
      l1_time_init(&cur->total_time);
      l1_priority_increase(&cur->priority_level);
    }
  }
}

/** Helper function:
 * Find next thread to run in RR and ensure it is at back of list before running
 */
l1_thread_info *mlfq_next_thread(l1_thread_list *list) {
  if (list == NULL)
    return NULL;
  // Traverse through list to find thread with highest priority
  l1_thread_info *max_prio_thread = list->head;
  for (l1_thread_info *cur = list->head; cur != NULL; cur = cur->next) {
    if (cur->priority_level > max_prio_thread->priority_level)
      max_prio_thread = cur;
  }
  return max_prio_thread;
}

/** Schedules threads according to mlfq policy */
l1_thread_info* l1_mlfq_policy(l1_thread_info* prev, l1_thread_info* next) {

  /* TODO: Your code */
  l1_scheduler_info *sched = get_scheduler();
  l1_thread_list *runnable = &sched->thread_arrays[RUNNABLE];
  
  /* Rule 5: Boost threads that did not run during SCHED_PERIOD slices */
  if (sched->sched_ticks == 0)
    mlfq_boost(runnable);
  
  /* Rule 4: Check whole time slice usage */
  l1_time cur_slice_time;
  l1_time_init(&cur_slice_time);
  l1_time_diff(&cur_slice_time, prev->slice_end, prev->slice_start);
  int used_whole_slice = l1_time_is_smaller_equal(
                            l1_priority_slice_size(prev->priority_level), 
                            cur_slice_time);

  /* Rule 6: Check total time usage */
  int above_threshold = l1_time_is_smaller( TIME_PRIORITY_THRESHOLD,\
                                            prev->total_time);

  /* Decrease thread priority if needed */
  if (used_whole_slice || above_threshold) {
    l1_priority_decrease(&prev->priority_level);
    l1_time_init(&prev->total_time);
    prev->got_scheduled = 0;
  }
  
  return mlfq_next_thread(runnable); 
}

