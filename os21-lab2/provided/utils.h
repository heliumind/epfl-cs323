/**
 * @file utils.h
 * @brief Simple functions to help students test their code.
 *
 * @author Adrien Ghosn
 *
 */
#pragma once
#include "mutex.h"
#include "spinlock.h"
#include "schedule.h"
#include "thread_info.h"

/**
 * @brief Free the global state
 *
 * @warning NOT THREAD SAFE
 */
void clean_up_global_state();

/**
 * @brief Does all the state initialization and starts executing your thread.
 *
 * Also wraps takes care of free everything once the execution is done.
 *
 * @warning NOT THREAD SAFE & assumes that the global state has been initialized.
 */
void initialize_and_launch(sched_policy policy, int sys_threads, thread_func_t your_main, void* args);


/**
 * @brief Returns the current thread.
 */
thread_info_t* get_current_thread();

/**
 * @brief Deschedules the current thread and unlocks the provided lock.
 *
 * This function is used to atomically release a mutex or a spinlock AFTER
 * de-scheduling the current thread.
 *
 * @Warning When this function returns, the current thread does not hold the 
 * provided lock!
 */
void cond_wait(void* lock, lock_type_t tpe);

/**
 * @brief unblocks a thread that is stuck on a mutex.
 *
 */
void tsafe_unblock_thread(thread_info_t* t);

