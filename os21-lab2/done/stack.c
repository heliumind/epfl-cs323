/**
 * @file stack.c
 * @brief Implementation of a basic stack for the students to use
 *
 * @author Mark Sutherland
 */
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

l2_stack* l2_stack_new(void) {
  l2_stack* l2_stack_new = (l2_stack*)malloc(sizeof(l2_stack));
  if( l2_stack_new == NULL ) 
    return NULL;
  l2_stack_new->capacity = MAX_STACK_CAPACITY;
  l2_stack_new->size = 0;
  l2_stack_new->base = (uint64_t*)malloc(l2_stack_new->capacity * sizeof(uint64_t));
  if( l2_stack_new->base == NULL ) {
    free(l2_stack_new);
    return NULL;
  }
  /* Point top to last element in the stack */
  l2_stack_new->top = l2_stack_new->base + (l2_stack_new->capacity);
  return l2_stack_new;
}

void l2_stack_free(l2_stack* thread_stack) {
  free(thread_stack->base);
  free(thread_stack);
}

bool l2_stack_is_full(l2_stack* thread_stack) {
  return (thread_stack->size == thread_stack->capacity);
}

bool l2_stack_is_empty(l2_stack* thread_stack) {
  return (thread_stack->size == 0);
}

void l2_stack_push(l2_stack* thread_stack, uint64_t data) {
  /* Decrement stack pointer and store data */
  thread_stack->top--;
  thread_stack->size++;
  *(thread_stack->top) = data;
}

uint64_t l2_stack_pop(l2_stack* thread_stack) {
  /* Read data and increment pointer */
  uint64_t data = *(thread_stack->top); 
  thread_stack->size--;
  thread_stack->top++;

  return data;
}
