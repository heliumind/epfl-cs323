/**
 * @brief API for mutexes
 *
 * @author Adrien Ghosn
 */
#pragma once
#include "spinlock.h"
#include "thread_info.h" 
#include "thread_list.h"

typedef struct {
  volatile tid_t owner;   /* Current owner of mutex, if free, DEFAULT_TARGET */
  spinlock_t lock;        /* Spinlock for mutual exclusion on the mutex state. */
  thread_list_t blocked;  /* List of threads blocked on this mutex. */
} mutex_t;

/**
 * @brief Initializes a mutex struct. 
 * Owner should be DEFAULT_TARGET.
 * lock should be initialized using the spinlock_init function.
 * blocked list should be initialized with thread_list_init.
 *
 * NOT THREAD SAFE.
 */
void mutex_init(mutex_t* m);

/**
 * @brief Locks a mutex m.
 *
 * Guarantees that when the function returns, the current thread owns m, i.e.,
 * m->owner == current thread's id.
 *
 * The spinlock lock is used for mutual exclusion on the mutex state.
 *
 * MUST BE THREAD SAFE.
 */
void mutex_lock(mutex_t* m);

/**
 * @brief Unlocks a mutex m.
 * This operation is allowed if and only if the mutex is currently owned 
 * by the current thread.
 *
 * If threads are blocked on the mutex, the current thread must transfer the mutex's
 * by setting m->owner to the correct value and calling tsafe_unblock_thread. 
 * provided in utils.h.
 *
 * Once again, m->lock is a spinlock used for mutual exclusion on the mutex's state.
 *
 * MUST BE THREAD SAFE.
 */
void mutex_unlock(mutex_t* m);
