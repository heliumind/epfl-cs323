/**
 * @brief Implementation of the priority API functions.
 *
 * @author Adrien Ghosn
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "priority.h"
#include "l2_time.h"

l2_time priority_slice_size(priority_t val) {
  assert(val >= LOWEST_PRIORITY && val <= TOP_PRIORITY);
  return MIN_SLICE * (TOP_PRIORITY - val + 1);
}

void priority_decrease(priority_t* p) {
  assert(p != NULL && *p >= LOWEST_PRIORITY);
  if (*p == LOWEST_PRIORITY) {
    return;
  }
  *p = *p - 1;
}

void priority_increase(priority_t* p) {
  assert(p != NULL && *p <= TOP_PRIORITY);
  if (*p == TOP_PRIORITY) {
    return;
  }
  *p = *p + 1;
}
