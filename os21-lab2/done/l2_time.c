/**
 * @brief Implementations for time in rdtsc and unix time
 *
 * @author Adrien Ghosn, Mark Sutherland
 */
#include <time.h>
#include "l2_time.h"


void l2_time_init(l2_time* t) {
  *t = 0;
}
  
void l2_time_get(l2_time* t) {
#ifdef USE_UNIX_TIME
    time(t);
#else
  unsigned int lo, hi; 
  asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
  *t = ((uint64_t)hi << 32) | lo;
#endif
}

void l2_time_diff(l2_time* result, l2_time end, l2_time start) {
   if (end < start) {
    *result = 1; 
    return;
   } 
   *result = end - start;
}

void l2_time_add(l2_time* accumulator, l2_time delta) {
  *accumulator += delta;
}

int l2_time_is_smaller(l2_time a, l2_time b) {
  return (a < b);
}

int l2_time_are_equal(l2_time a, l2_time b) {
  return (a == b);
}
