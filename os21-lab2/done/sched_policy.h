/**
 * @file sched_policy.c
 * @brief Header file for custom schedulers
 *
 * @author Atri Bhattacharyya, Mark Sutherland
 */
#pragma once
#include "thread_info.h"

thread_info_t* round_robin_policy(thread_info_t* prev, thread_info_t* next);

thread_info_t* smallest_cycles_policy(thread_info_t* prev, thread_info_t* next);

thread_info_t* mlfq_policy(thread_info_t* prev, thread_info_t* next);
