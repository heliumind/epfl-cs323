/**
 * @brief Definition for the priority values and API.
 *
 * @author Adrien Ghosn
 */
#pragma once
#include "l2_time.h"

/* Definition of priority value, TODO might change for an enum.*/
typedef int priority_t;

#define TOP_PRIORITY 10
#define LOWEST_PRIORITY 0

#define MIN_SLICE 1000

/* Maximum total time you can run at certain priority */
#define TIME_PRIORITY_THRESHOLD ((uint64_t)1000000)

/**
 * @brief Returns the value of a time slice at priority level val
 */
l2_time priority_slice_size(priority_t val);

/**
 * @brief decreases the priority level
 */
void priority_decrease(priority_t* p);

/**
 * @brief increases the priority level
 */
void priority_increase(priority_t* p);

