/**
 * @file schedule.h
 * @brief Header file for scheduler data structures/declarations
 *
 * @author Mark Sutherland
 */
#pragma once
#include "thread_info.h"
#include "thread_list.h"

/* Week 4: Interface for scheduling 
 * prev and next can both be null. 
 * None of them should be inside a queue.
 * None of them should be put in ANY scheduling queue.
 * If the scheduling policy selects a different thread than next, 
 * it should remove that thread from its queue.
 * If the provided next is not selected, the scheduler adds it back to correct queue.*/
typedef thread_info_t* (*sched_policy) (thread_info_t* prev, thread_info_t* next);

/* Answers the question "what does periodically mean?" in terms of sched_ticks*/
#define SCHED_PERIOD 10


/**
 * @brief Creates the sys threads using pthread, blocks them until all are ready
 * and joins on them afterwards.
 */
void init_sys_and_launch();

/**
 * @brief Adds a thread to the scheduler data structure in an associated
 * state.
 */
void add_to_scheduler(thread_info_t* thread, thread_state_t state);

/**
 * @brief The scheduler's main loop logic.
 *
 * This function is called by tsys and simulates a kernel scheduler.
 * tsys runs this loop as long as the runnable list is not empty.
 * The main loop logic is as follows: 
 * 1. Check the currently scheduled thread. If it has a yield target,
 * find the corresponding thread.
 * 2. If the current thread state is non-runnable, deschedule it.
 * 3. If the yield target is undefined, call the scheduler's select_next method.
 * 4. Change the state of the current thread.
 * 5. Schedule the next thread.
 * 6. Switch from tsys to the next thread.
 *
 */
void schedule();

/**
 * @brief handles cleanup for dead threads and joins
 * Returns 1 if current needs enqueuing or can be accessed, returns 0 otherwise.
 * If returns 0, current  should not be used once the function returns. 
 *
 * @warning THREAD SAFE 
 */
int handle_non_runnable(thread_info_t* current);

/**
 * @brief unblocks blocked thread and collects zombie if not null.
 *
 * This is a helper function that assumes blocked is in the JOINING list
 * and zombie is in ZOMBIE list or null. The function moves blocked
 * to the RUNNABLE list. If zombie is not null, it puts zombie into dead mode.
 * The free of the zombie happens in schedule.
 *
 * @warning The function changes errno value for a thread_info and moves 
 * structures from one list to another.
 * @warning The function expects the thread_info to be in the list
 * that corresponds to its state.
 *
 * @param blocked, not NULL, the blocked thread.
 * @param zombie, can be NULL, will be freed by the end of the function.
 *
 * @warning MIGHT LOCK RUNNABLE list
 */
void unblock_thread(thread_info_t* blocked, thread_info_t* zombie);

/**
 * @brief Yields to the scheduler
 * 
 * This function yields the processor to be able to schedule a different
 * green thread. The context switch to the next thread happens during this
 * function's execution.
 * 
 * The function will resume from this point at some later point in time as
 * decide by the scheduler (may be immediately).
 * 
 * yield(-1) yields to the system thread
 */
void yield(tid_t next);

/**
 * @brief switch_asm saves the current stack state in orig, and switches
 * to dest stack.
 *
 * This function is a small inlined assembly code that saves the scratch
 * registers rbx, r12, r13, r14, r15 on the current stack, saves the current
 * stack register into orig, and switches to dest. 
 * The asm will pop the dest's saved registers and return.
 */
void switch_asm(uint64_t* dest, uint64_t** orig); 
