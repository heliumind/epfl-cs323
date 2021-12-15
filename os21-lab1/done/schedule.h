/**
 * @file schedule.h
 * @brief Header file for scheduler data structures/declarations
 *
 * @author Mark Sutherland
 */
#pragma once
#include "thread_info.h"
#include "thread_list.h"

/* Week 4: Interface for scheduling */
typedef l1_thread_info* (*sched_policy) (l1_thread_info*, l1_thread_info*);

/* Answers the question "what does periodically mean?" in terms of sched_ticks*/
#define SCHED_PERIOD 10

typedef struct {
  l1_thread_info* current;                          /** Current thread */
  l1_tid next_tid;                                  /** Next thread */
  l1_thread_info* tsys;                             /** System thread */
  sched_policy select_next;                         /** Scheduler policy */
  l1_thread_list thread_arrays[NUM_THREAD_STATES];  /** Lists for the threads in different states. */
  uint64_t sched_ticks;                             /** Scheduler ticks */
} l1_scheduler_info;

/**
 * @brief initializes the scheduler and tsys.
 */
void initialize_scheduler(sched_policy policy);

/**
 * @brief Cleans up the scheduler
 */
void clean_up_scheduler();

/**
 * @brief get a reference to the scheduler
 */
l1_scheduler_info* get_scheduler();

/**
 * @brief Generate unique TIDs by incrementing scheduler variable 
 */
l1_tid get_uniq_tid();

/**
 * @brief Adds a thread to the scheduler data structure in an associated
 * state.
 */
void add_to_scheduler(l1_thread_info* thread, l1_thread_state state);

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
 */
void handle_non_runnable(l1_thread_info* current);

/**
 * @brief unblocks blocked thread and collects zombie if not null.
 *
 * This is a helper function that assumes blocked is in the BLOCKED list
 * and zombie is in ZOMBIE list or null. The function moves blocked
 * to the RUNNABLE list. If zombie is not null, it */
 // puts zombie into dead mode.
 // The free of the zombie happens in schedule.
 /**
 * @warning The function changes errno value for a l1_thread_info and moves 
 * structures from one list to another.
 * @warning The function expects the l1_thread_info to be in the list
 * that corresponds to its state.
 *
 * @param blocked, not NULL, the blocked thread.
 * @param zombie, can be NULL, will be freed by the end of the function.
 */
void unblock_thread(l1_thread_info* blocked, l1_thread_info* zombie);

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
void yield(l1_tid next);

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
