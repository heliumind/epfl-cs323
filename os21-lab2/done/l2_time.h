/**
 * @brief provides the definitions for real time in lab1.
 *
 * @author Adrien Ghosn, Mark Sutherland
 */
#pragma once
#include <stdint.h>
#include <time.h>

#define USE_UNIX_TIME

/* Definition of time type */
#ifdef USE_UNIX_TIME
typedef time_t l2_time;
#else
typedef uint64_t l2_time;
#endif

/**
 * @brief initializes time variable
 */
void l2_time_init(l2_time* t);

/**
 * @brief puts the current time in t.
 */
 void l2_time_get(l2_time* t);

/**
 * @brief Puts the time difference between start and end in result.
 */
void l2_time_diff(l2_time* result, l2_time end, l2_time start);

/**
 * @brief increment accumulator with the delta time.
 */
void l2_time_add(l2_time* accumulator, l2_time delta);

/** 
 * @brief return 1 if a < b, 0 if a >= b
 */
int l2_time_is_smaller(l2_time a, l2_time b);

/**
 * @brief return 1 if a == b, 0 otherwise
 */
int l2_time_are_equal(l2_time a, l2_time b);
