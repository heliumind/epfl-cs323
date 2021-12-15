/**
 * @brief Implementation of the priority API functions.
 *
 * @author Adrien Ghosn
 */
#include <stdio.h>
#include <stdlib.h>
#include "priority.h"
#include "l1_time.h"

l1_time l1_priority_slice_size(l1_priority val) {
  if (val < LOWEST_PRIORITY || val > TOP_PRIORITY) {
    fprintf(stderr, "Error: invalid priority value in l1_priority_slice_size\n");
    exit(-1);
  }
  return MIN_SLICE * (TOP_PRIORITY - val + 1);
}

void l1_priority_decrease(l1_priority* p) {
  if (p == NULL || *p < LOWEST_PRIORITY) {
    fprintf(stderr, "Error: invalid priority value in l1_priority_decrease\n");
    exit(-1);
  }
  if (*p == LOWEST_PRIORITY) {
    return;
  }
  *p = *p - 1;
}

void l1_priority_increase(l1_priority* p) {
  if (p == NULL || *p > TOP_PRIORITY) {
    fprintf(stderr, "Error: invalid priority value in l1_priority_increase\n");
    exit(-1);
  }
  if (*p == TOP_PRIORITY) {
    return;
  }
  *p = *p + 1;
}
