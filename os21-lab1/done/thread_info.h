/**
 * @file thread_info.h
 * @brief Header file that defines the abstraction of a "thread"
 *
 * @author Mark Sutherland
 */
#pragma once
#include <stdio.h>
#include <stdint.h>
#include "error.h"
#include "stack.h"
#include "l1_time.h"
#include "priority.h"

typedef  void*(*thread_func_t)(void*) ;

typedef enum  {
  SYSTHREAD=-1,         /* System thread, always runnable */
  RUNNING=0,            /* The thread is currently running */
  RUNNABLE,             /* The thread is not blocked on any other thread and 
                         * is ready to be run */
  BLOCKED,              /* Blocked on another process */
  ZOMBIE,               /* Zombie state waiting for one join */
  DEAD,                 /* Thread has been joined on and is ready to be collected */
  NUM_THREAD_STATES     
} l1_thread_state;
typedef uint32_t l1_tid;

typedef struct l1_thread_info {
  l1_tid id;                      /** Thread ID */
  l1_thread_state state;          /** Thread state */

  l1_tid joined_target;           /** Target for joining */
  l1_tid yield_target;            /** Target for yielding */

  thread_func_t thread_func;    /** Function being run by the thread */
  void *thread_func_args;         /** Function arg */
  
  l1_stack* thread_stack;         /** Thread stack */

  /* These pointers are used to link thread info structs into a list for the 
   * scheduler */
  struct  l1_thread_info* prev;   /** For thread scheduling */
  struct  l1_thread_info* next;   /** For thread scheduling */
  
  l1_error errno;                 /** Per-thread errno */
  void* retval;                   /** Value returned by the thread */
  void** join_recv;               /** Pointer to put joined thread's return val */

  /* Scheduling information */
  l1_priority priority_level;     /** Priority level for the scheduler */
  int got_scheduled;              /*When did it get scheduled */
  l1_time total_time;             /** Total execution time so far */
  l1_time slice_start;       /** Start time it was last scheduled */
  l1_time slice_end;         /**End time it was last descheduled */
} l1_thread_info;
