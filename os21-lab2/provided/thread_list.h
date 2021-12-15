/**
 * @file thread_list.h
 * @brief Header file for thread_list_t data structures/declarations
 *
 * @author Adrien Ghosn
 */

#pragma once
#include <stdbool.h>
#include "thread_info.h"

/**
 * @brief A simple representation of doubly linked list of threads.
 */ 
typedef struct thread_list_t {
  size_t size;
  thread_info_t* head;
  thread_info_t* tail;
} thread_list_t;


void thread_list_init(thread_list_t* list);

/**
 * @brief Removes thread from list.
 * @warning The function does not check that thread is in list.
 *
 * @arguments:
 *  list: a thread_list_t* that contains thread
 *  thread: a thread_info_t* to remove.
 *
 *@return thread_info_t* thread
 */
thread_info_t* thread_list_remove(thread_list_t* list, thread_info_t* thread);

/**
 * @brief Finds a thread in a list with a particular thread ID
 * 
 * @arguments:
 *  list: A thread_list_t* that contains thread with ID `tid`
 *  tid:  ID of the thread searched for
 *
 * @return thread_info_t* thread
 */
thread_info_t* thread_list_find(thread_list_t* list, tid_t tid);

/**
 * @brief Rotates the list by taking the head and putting it at the tail.
 * 
 * @returns The element moved to the tail
 */
thread_info_t* thread_list_rotate(thread_list_t* list);

/** 
 * @brief Adds a node to the list
 * 
 * @param list    The list to add to
 * @param thread  The thread to add 
 */
void thread_list_add(thread_list_t* list, thread_info_t* thread);

/** 
 * @brief Pop the head of the list
 * 
 * @param list    The list to pop from
 * @returns       The head of the input list
 */
thread_info_t* thread_list_pop(thread_list_t* list);

/** 
 * @brief Check if the list is empty
 * 
 * @param list    The list to check
 * @returns       True/false
 */
bool thread_list_is_empty(thread_list_t* list);
