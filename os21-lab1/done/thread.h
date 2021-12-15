/**
 * @file week03.h
 * @brief Header file for week 3 exercises
 *
 * @author Mark Sutherland
 */

#pragma once
#include "error.h"
#include "thread_info.h"

/* This is a function that calls a new thread's start_routine and stores the
 * return value in the function's info struct. This function is put on top
 * of the constructed stack for all new threads.
 */
void l1_start(void);

/**
 * @brief Spawns a new green thread
 *
 * This function implements functionality similar to, but not equalling,
 * libc's pthread_create. It is responsible for creating the new thread
 * and storing its ID in the `l1_tid` pointed to by the first 
 * argument. This is analogous to `pthread_t`.
 * 
 * Once run, a new thread will be ready to run the `start_routine` 
 * function.
 * 
 * @param  thread           Pointer to a `l1_tid`.
 * @param  start_routine    This is the function that will be run on the 
 *                          created thread. 
 * @return  If successful, return SUCCESS. On error, it returns an error code
 *          and the contents of *thread are undefined.
 */
l1_error l1_thread_create(l1_tid *thread, void *(*start_routine)(void *), void *arg);

/**
 * @brief Blocks until a thread completes
 * 
 * This function implements functionality similar to libc's `pthread_join`.
 * The function waits for the thread specified by thread to terminate.
 * If that thread has already terminated, then `l1_thread_join` returns 
 * immediately.
 * 
 * If `retval` is not NULL, then `l1_thread_join` copies the return value of 
 * the target thread into the location pointed to by `retval`. Multiple 
 * green threads should not try to join on the same target thread.
 * 
 * A possible error is if the target thread does not exist (ERRINVAL).
 * 
 * @param  target   Target thread's ID
 * @param  retval   This is where the return value of the target thread will
 *                  be put 
 * @return  If successful, return SUCCESS. On error, it returns an error code.
 */
l1_error l1_thread_join(l1_tid target, void **retval);
