/**
 * @file thread_list.h
 * @brief Header file for thread_list data structures/declarations
 *
 * @author Adrien Ghosn
 */

#pragma once
#include <stdbool.h>
#include "thread_info.h"

/**
 * @brief A simple representation of doubly linked list of threads.
 */ 
typedef struct l1_thread_list {
  size_t size;
  l1_thread_info *head;
  l1_thread_info *tail;
} l1_thread_list;

/**
 * @brief Removes thread from list.
 * @warning The function does not check that thread is in list.
 *
 * @arguments:
 *  list: a l1_thread_list* that contains thread
 *  thread: a l1_thread_info* to remove.
 *
 *@return l1_thread_info* thread
 */
l1_thread_info* thread_list_remove(l1_thread_list* list, l1_thread_info* thread);

/**
 * @brief Finds a thread in a list with a particular thread ID
 * 
 * @arguments:
 *  list: A l1_thread_list* that contains thread with ID `tid`
 *  tid:  ID of the thread searched for
 *
 * @return l1_thread_info* thread
 */
l1_thread_info* thread_list_find(l1_thread_list* list, l1_tid tid);

/**
 * @brief Rotates the list by taking the head and putting it at the tail.
 * 
 * @returns The element moved to the tail
 */
l1_thread_info* thread_list_rotate(l1_thread_list* list);

/** 
 * @brief Adds a node to the list
 * 
 * @param list    The list to add to
 * @param thread  The thread to add 
 */
void thread_list_add(l1_thread_list* list, l1_thread_info* thread);


/**
 * @brief Add a node to the list at the begining.
 *
 * @param list the list to add to
 * @param thread the thread to add.
 */
void thread_list_prepend(l1_thread_list* list, l1_thread_info* thread);

/** 
 * @brief Pop the head of the list
 * 
 * @param list    The list to pop from
 * @returns       The head of the input list
 */
l1_thread_info* thread_list_pop(l1_thread_list* list);

/** 
 * @brief Check if the list is empty
 * 
 * @param list    The list to check
 * @returns       True/false
 */
bool thread_list_is_empty(l1_thread_list* list);
