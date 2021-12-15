/**
 * @file week01.h
 * @brief Header file for week 1 exercises
 *
 * @author Atri Bhattacharyya, Adrien Ghosn
 */
#pragma once
#include <stdio.h>

/***** Task 1 : String comparison */

/**
 * @brief Compares two strings for dictionary order
 *
 * Taken from the man page:
 * The strcmp() function compares the two strings s1 and s2. It returns an
 * integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @param s1 Pointer to first string
 * @param s2 Pointer to second string
 *
 * @return Integer indicating comparison result
 * @author
 * @date
 */
int w1_strcmp(const char *s1, const char *s2);

/***** Task 2 : Linked list */

/**
 * @brief A linked list node
 */
typedef struct w1_node {
    int element;            /**< Each node contains an integer element */
    struct w1_node *next;   /**< And points to the next element in the list */
} w1_node;

/**
 * @brief Allocates new node with element = value
 *
 * Creates and returns a new linked list node. This function is 
 * responsible for allocating memory for the created node.
 *
 * @param value Value of element field in created node
 * @return If successful, pointer to created node
 *         On failure, NULL
 */
w1_node *w1_create_node(int value);
/**
 * @brief Deletes the node, freeing the memory
 *
 * @param Pointer to node to be deleted.
 */
void w1_delete_node(w1_node *node);
/**
 * @brief Inserts a node into a linked list at a specific position
 *
 * @param head Head node of the linked list. 
 *             Will be NULL to represent an empty list
 * @param node Node to be inserted
 * @param pos  Position to insert node
 *
 * @return If successful, head of the linked list after insertion
 *         On failure, NULL
 */
w1_node *w1_insert_node(w1_node *head, w1_node *node, unsigned pos);
/**
 * @brief Removes a node from a linked list
 *
 * @param head Head node of the linked list
 * @param node Node to be removed
 *
 * @return Head of the remaining list (NULL if empty), silent failure
 */
w1_node *w1_remove_node(w1_node *head, w1_node *node);
/**
 * @brief Counts size of list
 *
 * @param head Head node of the linked list
 * @return Number of elements in the list
 */
unsigned w1_size_list(w1_node *head);

/***** Task 3: binary tree traversals */

typedef struct Node {
  int data;
  struct Node* left;
  struct Node* right;
} Node;

/**
 * @brief prints to fd the post-order traversal of the binary tree rooted in node.
 *        For each element, use the format specifier "%d " for printing
 *
 * post-order is left, right, root.
 *
 * @param node, a pointer to the root of the tree.
 *        fd the output's file pointer.
 */
void w1_print_post_order(Node* node, FILE* fd);

/**
 * @brief prints to fd the pre-order traversal of the binary tree rooted in node.
 *        For each element, use the format specifier "%d " for printing
 *
 * pre-order is root, left, right.
 *
 * @param node, a pointer to the root of the tree.
 *        fd the output's file pointer.
 */
void w1_print_pre_order(Node* node, FILE* fd);
/**
 * @brief prints to fd the in-order traversal of the binary tree rooted in node.
 *        For each element, use the format specifier "%d " for printing
 *
 * in-order is left, root, right.
 *
 * @param node, a pointer to the root of the tree.
 *        fd the output's file pointer.
 */
void w1_print_in_order(Node* node, FILE* fd);

/***** Task 4: letter frequencies */

#define FREQ_LEN 26

typedef double w1_freq_t;

typedef w1_freq_t* w1_count_result_t;

/**
 * @brief computes the frequency of appeareance of letters in a text file.
 *
 * This function takes as argument the name of a file, and returns a w1_count_result_t 
 * with the frequency for each letter of the alphabet (non case-sensitive).
 * It is responsible for allocating memory, as required.
 *
 * @params file is the name of the text file.
 * @return a w1_count_result_t with the correct frequencies for each letter.
 */
w1_count_result_t w1_count_letter_freq(char* file);
