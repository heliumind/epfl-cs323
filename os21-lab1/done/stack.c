/**
 * @file stack.c
 * @brief Implementation of a basic stack for the students to use
 *
 * @author Mark Sutherland
 */
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

l1_stack* l1_stack_new(void) {
  l1_stack* l1_stack_new = (l1_stack*)malloc(sizeof(l1_stack));
  if( l1_stack_new == NULL ) 
    return NULL;
  l1_stack_new->capacity = MAX_STACK_CAPACITY;
  l1_stack_new->size = 0;
  l1_stack_new->base = (uint64_t*)malloc(l1_stack_new->capacity * sizeof(uint64_t));
  if( l1_stack_new->base == NULL ) {
    free(l1_stack_new);
    return NULL;
  }
  /* Point top to last element in the stack */
  l1_stack_new->top = l1_stack_new->base + (l1_stack_new->capacity);
  return l1_stack_new;
}

void l1_stack_free(l1_stack* thread_stack) {
  free(thread_stack->base);
  free(thread_stack);
}

bool l1_stack_is_full(l1_stack* thread_stack) {
  return (thread_stack->size == thread_stack->capacity);
}

bool l1_stack_is_empty(l1_stack* thread_stack) {
  return (thread_stack->size == 0);
}

void l1_stack_push(l1_stack* thread_stack, uint64_t data) {
  /* Decrement stack pointer and store data */
  thread_stack->top--;
  thread_stack->size++;
  *(thread_stack->top) = data;
}

uint64_t l1_stack_pop(l1_stack* thread_stack) {
  /* Read data and increment pointer */
  uint64_t data = *(thread_stack->top); 
  thread_stack->size--;
  thread_stack->top++;

  return data;
}
