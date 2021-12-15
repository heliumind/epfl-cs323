/**
 * @brief Definition for the priority values and API.
 *
 * @author Adrien Ghosn
 */
#pragma once
#include "l1_time.h"

/* Definition of priority value */
typedef int l1_priority;

#define TOP_PRIORITY 10
#define LOWEST_PRIORITY 0

/* Maximum total time you can run at certain priority */
#ifdef USE_UNIX_TIME
#define TIME_PRIORITY_THRESHOLD ((uint64_t)200)
#define MIN_SLICE 2
#else
#define TIME_PRIORITY_THRESHOLD ((uint64_t)1000000)
#define MIN_SLICE 1000
#endif

/**
 * @brief Returns the value of a time slice at priority level val
 */
l1_time l1_priority_slice_size(l1_priority val);

/**
 * @brief decreases the priority level
 */
void l1_priority_decrease(l1_priority* p);

/**
 * @brief increases the priority level
 */
void l1_priority_increase(l1_priority* p);

