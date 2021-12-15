/**
 * @file schedule.c
 * @brief Outlines in C for scheduler definitions
 *
 * @author Mark Sutherland
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <linux/futex.h>
#include "futex.h"
#include "schedule.h"
#include "scheduler_state.h"
#include "sys_thread.h"
#include "stack.h"
#include "thread.h"
#include "l2_time.h"
#include "mutex.h"


void* sys_thread_start(void* a) {
  scheduler_state_t* gstate = get_scheduler_state();
  l2_initialize_sys_thread();
  futex(&gstate->_start, FUTEX_WAIT, 0);
  schedule();
  l2_destroy_sys_thread();
  return NULL;
}

void init_sys_and_launch() {
  scheduler_state_t* gstate = get_scheduler_state();
  gstate->_start = 0;
  pthread_t tids[gstate->total_sys];
  for (int i = 0; i < gstate->total_sys; i++) {
    pthread_create(&tids[i], NULL, sys_thread_start, NULL); 
  }
  __sync_add_and_fetch(&gstate->_start, 1);
  futex(&gstate->_start,FUTEX_WAKE, gstate->total_sys);
  void *ptr = NULL;
  for (int i = 0; i < gstate->total_sys; i++) {
    pthread_join(tids[i], &ptr);
  }
}

/* Put yourself on the tail of the associated scheduler queue*/
void add_to_scheduler(thread_info_t* thread, thread_state_t state) {
  assert(thread != NULL);
  thread->prev = thread->next = NULL;
  thread->state = state;
  tsafe_enqueue_thread(thread, state);
}

/**
 * @brief always executes on tsys. 
 * Performs the scheduling logic:
 * 1. Tries to find a runnable thread. 
 * 2. If succeeds (thanks to select next or a yield, or a zombie finishing),
 * simply schedules the thread.
 * 3. If fails try to go to sleep. 
 */
void schedule() {
  sys_thread_t* scheduler = get_sys_thread();
  scheduler_state_t* gstate = get_scheduler_state();
  l2_time diff;
  thread_info_t* current = NULL;
  thread_info_t* next = NULL;
  thread_info_t* pnext = NULL;
  tid_t target = -1;
  while(1) {
    /* Health check ups, we verify that the sys state has not been corrupted */
    assert(scheduler != NULL);
    assert(scheduler->sys->state == SYSTHREAD);
    assert(scheduler->sys->prev == NULL && scheduler->sys->next == NULL);
    assert(scheduler->sys->yield_target == DEFAULT_TARGET);
    assert(scheduler->sys->joined_target == DEFAULT_TARGET);

    /* Increment this system thread scheduler ticks.
     * Note: This is thread local data, so safe without atomics.*/
    scheduler->ticks = (scheduler->ticks+1) % SCHED_PERIOD;

    /*We are begining and don't have anyone to schedule. */
    if (scheduler->current == NULL) {
      goto scheduling;
    } 

    current = scheduler->current;
    current->errno = SUCCESS;
    next = NULL;
    pnext = NULL;
    target = scheduler->current->yield_target;

    /* Timestamping logic */
    l2_time_get(&current->slice_end);
    l2_time_init(&diff);
    l2_time_diff(&diff, current->slice_end, current->slice_start);
    l2_time_add(&current->total_time, diff);

    if (current->state == RUNNING) {
      current->state = RUNNABLE;
    }

    /* Handle a yield */
    if (target != DEFAULT_TARGET) {
      assert(current->state == RUNNABLE);
      next = tsafe_find_and_remove_thread(target, RUNNABLE);
      if (next == NULL) {
        current->errno = ERRINVAL;
      }
      current->yield_target = DEFAULT_TARGET;
    }

    if (current->state == BLOCKED) {
      volatile void* lock = current->to_release;
      lock_type_t lock_tpe = current->lock_type;
      current->to_release = NULL;
      current->lock_type = NONE;
      current = NULL;
      if (lock != NULL) {
        switch(lock_tpe) {
          case SPINLOCK:
            spinlock_unlock(lock);
            break;
          case MUTEX:
            mutex_unlock((mutex_t*)lock);
            break;
          default:
            assert(0);
        }
      }
    }else {
      /* Check for corrupted state */
      assert(current->prev == NULL && current->next == NULL);
    }

    /* Enforce non global state */
    scheduler->current = NULL;
    
scheduling:
    /* Give a chance to the scheduling algorithm to bypass yield.
     * We save the one selected by schedule in case it needs to be rescheduled.
     * @warning At that point current might be null. */
    pnext = next;
    next = gstate->select_next(current, next);

    /* The original next (pnext) was not selected */
    if (pnext != next && pnext != NULL) {
      assert(pnext->state == RUNNABLE);
      tsafe_enqueue_thread(pnext, RUNNABLE); 
    }
   
    /* If we have a next, check that the scheduling policy respects the API */
    assert(next == NULL || 
        (next->prev == NULL && next->next == NULL && next->state == RUNNABLE));

    /* The current thread is blocking */
    if (current != NULL && (current->state == JOINING || current->state == ZOMBIE)) {
      current = handle_non_runnable(current)? current : NULL;
    } 
    
    /* Now it is safe to free the thread if it is dead */
    if (current != NULL && current->state == DEAD) {
        unregister_thread(current);
        l2_stack_free(current->thread_stack);
        free(current);
        current = NULL;
    }

     /* The thread is a yielder */
    if (current != NULL && current->state == RUNNABLE) {
      tsafe_enqueue_thread(current, RUNNABLE);
      current = NULL;
    }

    if (current != NULL && current->state == BLOCKED) {
      /* We do not need extra checks */
      current = NULL;
    }

    /* At that point current must be null. */
    assert(current == NULL);
   
    /* Nothing to schedule anymore? Try to block*/
    if (next == NULL) {
      lock_list(RUNNABLE);
      if (!thread_list_is_empty(&gstate->thread_arrays[RUNNABLE])) {
        unlock_list(RUNNABLE);
        current = NULL;
        next = NULL;
        pnext = NULL;
        target = DEFAULT_TARGET;
        sys_thread_wake_up();
        goto scheduling;
      }
      unlock_list(RUNNABLE);
      
      /* We are the last thread running, signal other to terminate. */
      if (__sync_add_and_fetch(&gstate->sleep_count, 0) == (gstate->total_sys-1)) {
        __sync_add_and_fetch(&gstate->sleep_count, IS_OVER);
        sys_thread_wake_up();
        return;
      }

      /* We are not the last thread, so we go to sleep */
      sys_thread_sleep();
      
      /* We woke up. Should we terminate? */
      if (__sync_add_and_fetch(&gstate->sleep_count, 0) >= IS_OVER) {
        return;
      }
      goto scheduling;
    }

    /* We successfully selected a next target to run. */
    scheduler->current = next;
    next->state = RUNNING;
    next->got_scheduled = 1;
    l2_time_init(&next->slice_end);
    l2_time_get(&next->slice_start);
    switch_asm((uint64_t*)next->thread_stack->top, (uint64_t**)&scheduler->sys->thread_stack->top);
  }
}

int handle_non_runnable(thread_info_t* current) {
  /* Health checks. */
  assert(current != NULL);
  assert(current->state == JOINING || current->state == ZOMBIE);
  assert(current->prev == NULL && current->next == NULL);

  scheduler_state_t* gstate = get_scheduler_state();
  assert(gstate != NULL);

  /* Thread called join */
  if (current->state == JOINING) {
    tid_t target = current->joined_target;
    /* Invalid target */
    if (target < 0) {
      unblock_thread(current, NULL);
      return 0;
    }
     
    /* Look for the target in zombie, runnable, and blocked lists
     * Note: we need to lock all three lists at once for consistency. */
    lock_all_lists();
    thread_info_t* joined = thread_list_find(&gstate->thread_arrays[ZOMBIE], target);
    if (joined != NULL) {
      thread_list_remove(&gstate->thread_arrays[ZOMBIE], joined);
      unlock_all_lists();
      unblock_thread(current, joined);
      return 0;
    }
    joined = does_thread_exists(target);
    if (joined != NULL) {
      /* The joined target is blocked too so we cannot do anything.*/
      thread_list_add(&gstate->thread_arrays[JOINING], current);
      unlock_all_lists();
      return 0;
    }

    /* We failed to block so we reschedule current */
    unlock_all_lists();
    unblock_thread(current, NULL);
    return 0;
  }

  /* We are a zombie and need to unblock people. */
  assert(current->state == ZOMBIE);
  
  /* Lock zombie and joining lists */
  lock_zombie_joining();

  thread_info_t* bl = gstate->thread_arrays[JOINING].head;
  thread_info_t* joined = NULL;
  while(bl != NULL) {
    thread_info_t* to_rm = bl;
    bl = bl->next;
    if (to_rm->joined_target == current->id) {
      thread_list_remove(&gstate->thread_arrays[JOINING], to_rm);
      if (joined == NULL) {
        joined = to_rm;
      } else {
        unblock_thread(to_rm, NULL);
      }
    }
  }
  /* current has been joined on successfully, it is now dead.
   * Return 1 to let the scheduler know that it should be freed.*/
  if (joined != NULL) {
   unblock_thread(joined, current);
   unlock_zombie_joining();
   return 1;
  }
  /* We are a zombie, we can add ourselves to the list */
  thread_list_add(&gstate->thread_arrays[ZOMBIE], current);
  unlock_zombie_joining();  
  return 0; 

}

void unblock_thread(thread_info_t* blocked, thread_info_t* zombie) {
  /* Health checks */
  assert(blocked != NULL);
  assert(blocked->state == JOINING);
  assert(blocked->prev == NULL && blocked->next == NULL);
  assert(zombie == NULL || 
      (zombie->state == ZOMBIE && zombie->prev == NULL && zombie->next == NULL));
  
  /* Spurious wake up */
  if (!zombie) {
    blocked->state = RUNNABLE;
    blocked->errno = ERRINVAL;
    blocked->joined_target = DEFAULT_TARGET;
    tsafe_enqueue_thread(blocked, RUNNABLE);
    return;
  }

  if (blocked->join_recv != NULL) {
    *(blocked->join_recv) = zombie->retval; 
  }
  blocked->state = RUNNABLE;
  blocked->errno = SUCCESS;
  blocked->joined_target = DEFAULT_TARGET;
  tsafe_enqueue_thread(blocked, RUNNABLE);
  /* Mark as dead to free it in schedule */
  zombie->state = DEAD;
}

void yield(tid_t tid) {
  /* Setup the target */
  sys_thread_t* scheduler = get_sys_thread();
  scheduler->current->yield_target = tid;

  /* Always go back to sys */
  switch_asm((uint64_t*)scheduler->sys->thread_stack->top,
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
