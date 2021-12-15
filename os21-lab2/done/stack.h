/**
 * @file stack.h
 * @brief Header file that defines the API for our basic stack
 *
 * @author Mark Sutherland
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

/* Each thread allocates a stack of capacity = MAX_STACK_CAPACITY words
 * On a 64-bit architecture, a word is 64 bits */
#define MAX_STACK_CAPACITY  1024

/**
 * @brief A stack structure
 */
typedef struct {
  unsigned capacity;  /** Capacity of the stack (in 64-bit chunks) */
  unsigned size;      /** Used space in the stack */
  uint64_t *top;      /** Pointer to the "top" of the stack */
  uint64_t *base;     /** Pointer to the base of the allocated region */
} l2_stack;

/**
 * @brief Creates a new stack for a thread
 *
 * This function allocates space for and sets up a new stack of type 
 * `l2_stack` and of capacity MAX_STACK_CAPACITY.
 *
 * @return  A pointer to the allocated stack. Returns NULL if unable
 *          to allocate space for the stack
 */
l2_stack* l2_stack_new(void);

/**
 * @brief Cleans up the stack for a thread on completion 
 * 
 * @param   thread_stack A pointer to the stack to be freed
 */
void l2_stack_free(l2_stack* thread_stack);

/**
 * @brief Check if the stack is full 
 *
 * @return Whether the stack is full
 */
bool l2_stack_is_full(l2_stack* thread_stack);

/**
 * @brief Check if the stack is empty 
 *
 * @return Whether the stack is empty
 */
bool l2_stack_is_empty(l2_stack* thread_stack);
/**
 * @brief Push 64 bits onto the stack
 *
 * @param thread_stack  The stack to push data onto
 * @param data          The data to push on the stack
 */
void l2_stack_push(l2_stack* thread_stack, uint64_t data);
/**
 * @brief Pop 64 bits from the stack
 *
 * @param   thread_stack  The stack to pop from
 * @return  Return the top 64 bits of the stack
 */
uint64_t l2_stack_pop(l2_stack* thread_stack);
