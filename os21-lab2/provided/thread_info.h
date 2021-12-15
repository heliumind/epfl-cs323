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
#include "l2_time.h"
#include "priority.h"
#include "spinlock.h"

#define DEFAULT_TARGET (-1)

typedef enum  {
  SYSTHREAD=-1,         /* System thread, always runnable */
  RUNNING=0,            /* The thread is currently running */
  RUNNABLE,             /* The thread is not blocked on any other thread and 
                         * is ready to be run */
  JOINING,              /* Joining on another process */
  BLOCKED,              /* Blocked until rescheduled. */
  ZOMBIE,               /* Zombie state waiting for one join */
  DEAD,                 /* Thread has been joined on and is ready to be collected */
  NUM_THREAD_STATES     
} thread_state_t;

/* Identifier for a thread. Should be unique */
typedef uint32_t tid_t;

typedef void* (*thread_func_t)(void*);

typedef struct thread_info_t {
  tid_t id;                      /** Thread ID */
  thread_state_t state;          /** Thread state */

  tid_t joined_target;           /** Target for joining */
  tid_t yield_target;            /** Target for yielding */

  thread_func_t thread_func;    /** Function being run by the thread */
  void *thread_func_args;         /** Function arg */
  
  l2_stack* thread_stack;         /** Thread stack */

  /* These pointers are used to link thread info structs into a list for the 
   * scheduler */
  struct  thread_info_t* prev;   /** For thread scheduling */
  struct  thread_info_t* next;   /** For thread scheduling */

  struct thread_info_t* ex_prev; /* scheduler keeping track of existing threads.*/
  struct thread_info_t* ex_next; 
  
  l2_error errno;                 /** Per-thread errno */
  void* retval;                   /** Value returned by the thread */
  void** join_recv;               /** Pointer to put joined thread's return val */
  void* channel_buffer;          /** Pointer to value received/sent on a channel */

  /* Scheduling information */
  priority_t priority_level;     /** Priority level for the scheduler */
  int got_scheduled;              /*Did it get scheduled at this priority */
  l2_time total_time;             /** Total execution time so far */
  l2_time slice_start;       /** Start time it was last scheduled */
  l2_time slice_end;         /**End time it was last descheduled */

  volatile void* to_release;   /* Pointer to a lock that needs to be released.*/
  lock_type_t lock_type;
} thread_info_t;
