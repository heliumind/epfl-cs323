/**
 * @brief API for simple spinlocks.
 *
 * @author Adrien Ghosn
 */
#pragma once
#include "locks.h"

/**
 * @brief initializes a lock to UNLOCKED
 * Does not have to be THREAD SAFE.
 */
void spinlock_init(spinlock_t* lock); 

/**
 * @brief Locks the spinlock lock.
 *
 * This function must perform a __sync_val_compare_and_swap on lock, expecting
 * the value UNLOCKED, and trying to put the value LOCKED, until success.
 *
 * MUST BE THREAD SAFE.
 */
void spinlock_lock(spinlock_t* lock);

/**
 * @brief Unlocks the spinlock lock.
 *
 * This must put back the UNLOCKED value in the spinlock ATOMICALLY.
 * Does not require ownership tests.
 *
 * MUST BE THREAD SAFE.
 */
void spinlock_unlock(spinlock_t* lock);
