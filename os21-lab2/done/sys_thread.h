/**
 * @brief API for a system thread.
 *
 * @author Adrien Ghosn
 */
#pragma once
#include "schedule.h"

typedef struct {
  uint64_t ticks;           /* ticks from that scheduler */
  thread_info_t* current;  /* The current thread scheduled on that pthread */
  thread_info_t* sys;      /* This thread's representation */
} sys_thread_t;

/**
 * @brief initializes the sys_thread local to the current thread.
 *
 * SHOULD BE CALLED ONCE, NOT THREAD SAFE
 */
void l2_initialize_sys_thread();

/**
 * @brief Free the local_sys_thread.
 *
 * @warning assumes that it was allocated.
 */
void l2_destroy_sys_thread();

/**
 * @brief returns this thread's system thread.
 *
 * THREAD SAFE since it is stored in thread local storage.
 */
sys_thread_t* get_sys_thread();

/**
 * @brief sends a sys thread to sleep on a futex.
 * If this is the last thread, we reject the call to sleep.
 *
 * THREAD SAFE
 */
void sys_thread_sleep();

/**
 * @brief wakes up all the blocked threads on global_state->sleep_count.
 *
 * THREAD SAFE
 */
void sys_thread_wake_up();
