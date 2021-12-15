/**
 * @brief Implementations for time in rdtsc and unix time
 *
 * @author Adrien Ghosn, Mark Sutherland
 */
#include <time.h>
#include "l1_time.h"


void l1_time_init(l1_time* t) {
  *t = 0;
}
  
void l1_time_get(l1_time* t) {
#ifdef USE_UNIX_TIME
    time(t);
#else
  unsigned int lo, hi; 
  asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
  *t = ((uint64_t)hi << 32) | lo;
#endif
}

void l1_time_diff(l1_time* result, l1_time end, l1_time start) {
   if (end < start) {
    *result = 1; 
    return;
   } 
   *result = end - start;
}

void l1_time_add(l1_time* accumulator, l1_time delta) {
  *accumulator += delta;
}

int l1_time_is_smaller(l1_time a, l1_time b) {
  return (a < b);
}

int l1_time_are_equal(l1_time a, l1_time b) {
  return (a == b);
}
