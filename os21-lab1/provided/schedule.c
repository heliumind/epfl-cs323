/**
 * @file schedule.c
 * @brief Outlines in C for scheduler definitions
 *
 * @author Mark Sutherland
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"
#include "stack.h"
#include "thread.h"
#include "l1_time.h"

l1_scheduler_info* scheduler = NULL; 

void initialize_scheduler(sched_policy policy) {
  scheduler = (l1_scheduler_info*) malloc(sizeof(l1_scheduler_info));
  if (!scheduler) {
    fprintf(stderr, "Error: scheduler not initialized!\n");
    exit(-1);
  }
 
  /* Initialize scheduler */
  memset(scheduler, 0, sizeof(l1_scheduler_info)); 
  scheduler->next_tid = 0;
  
  /* Create tsys */
  scheduler->tsys = malloc(sizeof(l1_thread_info));
  if (!scheduler->tsys) {
    fprintf(stderr, "Unable to create original system thread!\n");
    exit(-1);
  }
  memset(scheduler->tsys, 0, sizeof(l1_thread_info));
  scheduler->tsys->state = SYSTHREAD;
  scheduler->tsys->got_scheduled = 0;
  scheduler->tsys->id = -1;
  scheduler->current = scheduler->tsys;
  scheduler->tsys->yield_target = -1;
  scheduler->tsys->thread_stack = malloc(sizeof(l1_stack));
  scheduler->select_next = policy;
  scheduler->sched_ticks = 0;
}

void clean_up_scheduler() {
  if (scheduler == NULL) {
    return;
  }
  /* Free system thread */
  if (scheduler->tsys) {
    free(scheduler->tsys->thread_stack);
    scheduler->tsys->thread_stack = NULL;
  }
  /* Free the scheduler */
  free(scheduler);
  scheduler = NULL;
}

l1_scheduler_info* get_scheduler() {
  return scheduler;
}

l1_tid get_uniq_tid(){
  return scheduler->next_tid++;
}

/* Put yourself on the tail of the associated scheduler queue*/
void add_to_scheduler(l1_thread_info* thread, l1_thread_state state) {
  if (!thread) {
    fprintf(stderr, "Error: trying to add NULL to scheduler thread list!\n");
    exit(-1);
  }
  thread->prev = thread->next = NULL;
  thread->state = state;
  thread_list_add(&scheduler->thread_arrays[state], thread);
}

/**
 * @brief always executes on tsys
 */
void schedule() {
  while(!thread_list_is_empty(&scheduler->thread_arrays[RUNNABLE])) {
    if (scheduler == NULL || scheduler->current  == NULL) {
      fprintf(stderr, "Error: null pointer in scheduler logic.\n");
      exit(-1);
    }

    if (scheduler->tsys->state != SYSTHREAD 
        || scheduler->tsys->prev != NULL
        || scheduler->tsys->next != NULL) {
      fprintf(stderr, "Error: bad handling of tsys\n");
      exit(-1);
    }

    l1_thread_info* current = scheduler->current;
    l1_thread_info* next = NULL;
    l1_tid target = scheduler->current->yield_target;

    /*Timestamp the end of slice*/
    l1_time_get(&current->slice_end);
    l1_time diff;
    l1_time_diff(&diff, current->slice_end, current->slice_start);
    l1_time_add(&current->total_time, diff);

    /* Enforce non-global state */
    scheduler->current = NULL;
    current->errno = SUCCESS;
    
    if (current->state == RUNNING) {
      current->state = RUNNABLE;
      if (target != -1) {
        next = thread_list_find(&scheduler->thread_arrays[RUNNABLE], target);
        if (next == NULL) {
          current->errno = ERRINVAL;
        }
      }
      /* fake rotate the list. */
      thread_list_remove(&scheduler->thread_arrays[RUNNABLE], current);
      thread_list_add(&scheduler->thread_arrays[RUNNABLE], current);
    }

    /* The thread is blocking */
    if (current != scheduler->tsys && 
        (current->state == BLOCKED || current->state == ZOMBIE)) {
      handle_non_runnable(current);
    }
    /* Give a chance to the scheduling algorithm to bypass yield*/
    next = scheduler->select_next(current, next);

    /* Now it is safe to free the thread if it is dead */
    if (current->state == DEAD) {
      l1_stack_free(current->thread_stack);
      free(current);
      current = NULL;
    }
   
    /* Nothing to schedule anymore.*/
    if (next == NULL) {
      break;
    }
    scheduler->current = next;
    next->state = RUNNING;
    next->got_scheduled = scheduler->sched_ticks;
    /*Make the thread the head of the list*/
    thread_list_remove(&scheduler->thread_arrays[RUNNABLE], next);
    thread_list_prepend(&scheduler->thread_arrays[RUNNABLE], next);
    l1_time_init(&next->slice_end);
    l1_time_get(&next->slice_start);
    
    /* Scheduler ticks */
    scheduler->sched_ticks = (scheduler->sched_ticks+1) % SCHED_PERIOD;
    switch_asm((uint64_t*)next->thread_stack->top, (uint64_t**)&scheduler->tsys->thread_stack->top);
  }
  printf("Program terminating!\n"); 
}

void handle_non_runnable(l1_thread_info* current) {
  if (!current) {
    fprintf(stderr, "Error: current is null in handle_non_runnable.\n");
    exit(-1);
  }
  if (current->state != BLOCKED && current->state != ZOMBIE) {
    fprintf(stderr, "Error: handle_non_runnable called  with invalid state.\n");
    exit(-1);
  }

  /* Move the thread to the appropriate list */
  thread_list_remove(&scheduler->thread_arrays[RUNNABLE], current);
  thread_list_add(&scheduler->thread_arrays[current->state], current);

  /* Thread called join */
  if (current->state == BLOCKED) {
    l1_tid target = current->joined_target;
    /* Invalid target */
    if (target == -1) {
      unblock_thread(current, NULL);
      return;
    }

    /* Look for the target in zombie, runnable, and blocked lists */
    l1_thread_info* joined = thread_list_find(&scheduler->thread_arrays[ZOMBIE], target);
    if (joined != NULL) {
      unblock_thread(current, joined);
      return;
    }
    joined = thread_list_find(&scheduler->thread_arrays[BLOCKED], target);
    if (joined != NULL) { 
      return;
    }
    
    joined = thread_list_find(&scheduler->thread_arrays[RUNNABLE], target);
    if (joined != NULL) { 
      return;
    }
    unblock_thread(current, NULL);
    return;
  }

  /* We are a zombie and need to unblock people. */
  l1_thread_info* bl = scheduler->thread_arrays[BLOCKED].head;
  l1_thread_info* joined = NULL;
  while(bl != NULL) {
    if (bl->joined_target == current->id) {
      l1_thread_info* to_rm = bl;
      if (joined == NULL) {
        joined = to_rm;
      } else {
        unblock_thread(to_rm, NULL);
      }
    }
    bl = bl->next;
  }
  if (joined != NULL) {
   unblock_thread(joined, current);
  }
}

void unblock_thread(l1_thread_info* blocked, l1_thread_info* zombie) {
  if (!blocked) {
    fprintf(stderr, "Error: unblock_thread called with null blocked.\n");
    exit(-1);
  }
  if (blocked->state != BLOCKED) {
    fprintf(stderr, "Error: unblock_thread blocked thread invalid thread state\n");
  }
  if (zombie != NULL && zombie->state != ZOMBIE) {
    fprintf(stderr, "Error: unblock_thread zombie thread invalid state\n");
    exit(-1);
  }
  /* Spurious wake up */
  if (!zombie) {
    blocked->state = RUNNABLE;
    blocked->errno = ERRINVAL;
    blocked->joined_target = -1;
    thread_list_remove(&scheduler->thread_arrays[BLOCKED], blocked);
    thread_list_add(&scheduler->thread_arrays[RUNNABLE], blocked);
    return;
  }

  if (blocked->join_recv != NULL) {
    *(blocked->join_recv) = zombie->retval; 
  }
  blocked->state = RUNNABLE;
  blocked->errno = SUCCESS;
  blocked->joined_target = -1;
  thread_list_remove(&scheduler->thread_arrays[BLOCKED], blocked);
  thread_list_add(&scheduler->thread_arrays[RUNNABLE], blocked);
  thread_list_remove(&scheduler->thread_arrays[ZOMBIE], zombie);
  /* Mark as dead to free it in schedule */
  zombie->state = DEAD;
}

void yield(l1_tid tid) {
  /* Setup the target */
  scheduler->current->yield_target = tid;

  /* Always go back to tsys */
  switch_asm((uint64_t*)scheduler->tsys->thread_stack->top,
              (uint64_t**)&scheduler->current->thread_stack->top);
  /* Nothing to do, we are rescheduled.  */
}


void switch_asm(uint64_t* dest, uint64_t** orig) {
  asm volatile(
      "mov %%rsp, (%[orig])\n"
      "mov %[dest], %%rsp\n"
      : /* No outputs */
      : [orig] "r"(orig), [dest] "r"(dest)
      : "rbx", "r12", "r13", "r14", "r15", "memory" /* clobbers */
      ); 
}
