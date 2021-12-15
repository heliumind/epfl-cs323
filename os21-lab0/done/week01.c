/**
 * @file week01.c
 * @brief Exercises in C for the first week
 *
 * A few exercises, marked by `TODO`s.
 * 1. Write a string comparison function, with the same semantics as the
 *    standard `strcmp`.
 * 2. Debug the linked-list implementation.
 * 3. Implement binary tree traversals in three orders: in-order, 
 *    preorder, postorder.
 * 4. Count the occurence of letters in a file.
 *
 * @author Atri Bhattacharyya, Adrien Ghosn
 */
#include <stdlib.h>
#include <ctype.h>
#include "week01.h"

int w1_strcmp(const char *s1, const char *s2) {
  /* TODO: Replace with your own code */

 while (*s1 != '\0' && (*s1 == *s2)) {
    s1++;
    s2++;
  }

  if (*s1 == *s2) {
    return 0;
  } else if (*s1 < *s2) {
    return -1;
  } else {
    return 1;
  }
}

/* TODO: Debug the linked list implementation. The API consists of five 
 * functions:
 * - w1_create_node
 * - w1_delete_node
 * - w1_insert_node
 * - w1_remove_node 
 * - w1_size_list
 * Hint: There are two bugs.
 */
w1_node *w1_create_node(int value) {
  w1_node *node = (w1_node *)calloc(1, sizeof(w1_node));

  if(node == NULL)
    return NULL;
  
  node->element = value;
  node->next = NULL;
  return node;
}

void w1_delete_node(w1_node *node) {
  free(node);
}

w1_node *w1_insert_node(w1_node *head, w1_node *node, unsigned pos) {

  if(node == NULL)
    return NULL;

  /* Inserting into an empty list */
  if(head == NULL){
    /* Only acceptable position is 0 */
    if(pos == 0)
      return node;
    /* All other positions are illegal */
    else
      return NULL;
  }

  /* Non-empty list */
  if(pos == 0) {
    node->next = head;
    return node;
  }
  /* Parse through list till position pos */
  w1_node *prev = head;
  for(unsigned i = 1; i < pos; i++){
    prev = prev->next;
    /* This means end of list encountered before pos */
    if(prev == NULL)
      return NULL;
  }
  node->next = prev->next;
  prev->next = node;
  return head;
}

w1_node *w1_remove_node(w1_node *head, w1_node *node) {
  /* Silent failure for empty list */
  if(head == NULL || node == NULL)
    return NULL;

  /* Removing first node changes head */
  if(node == head) 
    head = head->next;
  else {
    w1_node *prev = head;
    while((prev != NULL) && (prev->next != node))
      prev = prev->next;
    /* Silent failure if end of list reached before finding node */
    if(prev == NULL)
      return NULL;
    prev->next = node->next;
  }
  node->next = NULL;

  return head;
}

unsigned w1_size_list(w1_node *head) {
  unsigned size = 0;

  while(head != NULL) {
    size++;
    head = head->next;
  }
  return size;
}

Node* newNode(int data) {
  Node *node = (Node *) calloc(1, sizeof(Node));
  if (node == NULL)
    return NULL;
  node->data = data;
  node->left = NULL;
  node->right = NULL;

  return node;
}

void w1_print_post_order(Node* node, FILE* fd) {
  /* TODO: Replace with your own code */
  if (node == NULL)
    return;
  
  // first recur on left subtree
  w1_print_post_order(node->left, fd);

  // then recur on right subtree
  w1_print_post_order(node->right, fd);

  // now deal with node
  fprintf(fd, "%d ", node->data);
}

void w1_print_pre_order(Node* node, FILE* fd) {
  /* TODO: Replace with your own code */
  if (node == NULL)
    return;
  
  // first print data of node
  fprintf(fd, "%d ", node->data);

  // then recur on left subtree
  w1_print_pre_order(node->left, fd);

  // now recur on right subtree
  w1_print_pre_order(node->right, fd);
}
void w1_print_in_order(Node* node, FILE* fd) {
  /* TODO: Replace with your own code */
  if (node == NULL)
    return;
  
  // first recur on left subtree
  w1_print_in_order(node->left, fd);

  // then print data of node
  fprintf(fd, "%d ", node->data);

  // now recur on right subtree
  w1_print_in_order(node->right, fd);
}

w1_count_result_t w1_count_letter_freq(char* file) {
  /* TODO: Replace with your own code */
  /* Open text file */
  FILE *fp = fopen(file, "r");
  /* Allocate and initialize count result */
  w1_count_result_t result = (w1_freq_t *) calloc(FREQ_LEN, sizeof(w1_freq_t));

  if (fp != NULL) {
    /* Traverse character by character through file */
    char read;
    int total_count = 0;
    while ((read = fgetc(fp)) != EOF) {
      /* Increment the count of each letter by one */
      total_count++;
      if (isalpha(read))
        result[tolower(read) - 'a']++;
    }
    /* Close text file */
    fclose(fp);

    /* Calculate frequency from occurrence */
    // Check if file is not empty, avoid dividing by zero
    if (total_count != 0) { 
      for (int i = 0; i < FREQ_LEN; i++)
        result[i] /= total_count;
    }
  } else {
    printf("No file %s\n", file);
  }
  return result;
}
