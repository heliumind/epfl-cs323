/**
 * @brief Implementation of spinlock.
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include <stdlib.h>
#include "spinlock.h"

void spinlock_init(spinlock_t* lock) {
  /* TODO: Implement */
  *lock = UNLOCKED;
}

void spinlock_lock(spinlock_t* lock) {
  /* TODO: Implement */
  while (__sync_val_compare_and_swap(lock, UNLOCKED, LOCKED) == LOCKED)
    ; // spin
}

void spinlock_unlock(spinlock_t* lock) {
  /* TODO: Implement */
  if (__sync_val_compare_and_swap(lock, LOCKED, UNLOCKED) == UNLOCKED)
    assert(*lock != UNLOCKED);
}
