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
typedef time_t l1_time;
#else
typedef uint64_t l1_time;
#endif

/**
 * @brief initializes time variable
 */
void l1_time_init(l1_time* t);

/**
 * @brief puts the current time in t.
 */
 void l1_time_get(l1_time* t);

/**
 * @brief Puts the time difference between start and end in result.
 */
void l1_time_diff(l1_time* result, l1_time end, l1_time start);

/**
 * @brief increment accumulator with the delta time.
 */
void l1_time_add(l1_time* accumulator, l1_time delta);

/** 
 * @brief return 1 if a < b, 0 if a >= b
 */
int l1_time_is_smaller(l1_time a, l1_time b);

/**
 * @brief return 1 if a == b, 0 otherwise
 */
int l1_time_are_equal(l1_time a, l1_time b);
