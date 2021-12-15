/**
 * @file sched_policy.c
 * @brief Header file for custom schedulers
 *
 * @author Atri Bhattacharyya, Mark Sutherland
 */
#pragma once
#include "thread_info.h"

l1_thread_info* l1_round_robin_policy(l1_thread_info* prev, l1_thread_info* next);

l1_thread_info* l1_smallest_cycles_policy(l1_thread_info* prev, l1_thread_info* next);

l1_thread_info* l1_mlfq_policy(l1_thread_info* prev, l1_thread_info* next);
