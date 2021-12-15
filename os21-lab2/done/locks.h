/**
 * @brief file that declares common type and values for locks.
 *
 * @author Adrien Ghosn.
 */
#pragma once
#include <stdbool.h>

typedef volatile bool spinlock_t;

#define UNLOCKED false
#define LOCKED true

typedef enum {
  NONE,
  SPINLOCK,
  MUTEX
} lock_type_t;
