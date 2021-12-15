/**
 * @file tests.c
 * @brief Unit-test suite for lab 0 exercises
 *
 * A simple test suite for lab 0 exercises. Provides a template which
 * students should add tests to.
 *
 * @author Atri Bhattacharyya, Adrien Ghosn, Ahmad Hazimeh
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "week01.h"
#include "week02.h"

/* This is an example of using the unit testing framework `check`.
 * There are two components to each test:
 * - The test definition: This contains the work to be done during the test
 *   and contains `assert`s to check that specific conditions hold at runtime.
 *   Test definitions are written using the `START_TEST()` and `END_TEST`
 *   macros. An example is shown below (`example_test`).
 * - Adding the test to the test suite: This tells the framework to which
 *   tests to run. For example, the line `tcase_add_test(tc1, example_test);`
 *   tells the framework to run the example test.
 *
 * You are strongly encouraged to use the `check` framework to test your code.
 * Comprehensive documentation for the framework is available at
 * `https://libcheck.github.io/check/`.
 * Define further tests using the `START_TEST`, `END_TEST` macros and add them
 * to the test suite in `main()`.
 */

/* This test fails if ANY of the asserts fails */
START_TEST(example_test) {

  /* Example integer test.
   * This checks whether the variable `num` is less than 1000 at runtime. */
  int num = 1;
  ck_assert_int_le(num, 1000);

  /* Example pointer check.
   * This checks whether the variable `new_node` is NULL at runtime. */
  w1_node *new_node = w1_create_node(0);
  ck_assert_ptr_ne(new_node, NULL);
  ck_assert_int_eq(new_node->element, 0);
  ck_assert_ptr_eq(new_node->next, NULL);

  /* Testing insertion into linked list.
   * A `NULL` head pointer signifies an empty node.
   * Inserting a node into an empty list at position 0 makes the
   * new node the head. This is checked by the assert statement. */
  w1_node *head = NULL, *new_head;
  new_head = w1_insert_node(head, new_node, 0);
  ck_assert_ptr_eq(new_head, new_node);

  /* Clean-up */
  w1_delete_node(new_node);
}
END_TEST

/* print_hello prints Hello World */
void print_hello() {
    printf("Hello world, %d\n", getpid());
}

START_TEST(example_test2) {
    /* This should call print_hello twice,
     * printing "Hello world, ____" with different PIDs */
    bool root[] = {true, true, false, false, false};
    w2_fork(root, 0, print_hello);
}
END_TEST

START_TEST(w1_strcmp_test) {
  char s1[] = "ETHL";
  char s2[] = "ETHZ";

  /* Case 1: Content of both strings are equal */
  int result1 = w1_strcmp(s1, s1);
  ck_assert_int_eq(result1, 0);

  /* Case 2: First character that does not match has lower value in
  s1 than in s2 */
  int result2 = w1_strcmp(s1, s2);
  ck_assert_int_lt(result2, 0);

  /* Case 3: First character that does not match has greater value in
  s1 than in s2 */
  int result3 = w1_strcmp(s2, s1);
  ck_assert_int_gt(result3, 0);
}
END_TEST

START_TEST(w1_insert_node_test) {
  w1_node *node0 = w1_create_node(0);
  w1_node *node1 = w1_create_node(1);
  w1_node *head = node0, *new_head;
  node0->next = node1;

  /* Case 1: Insert at position 1 */
  w1_node *new_node1 = w1_create_node(2);
  new_head = w1_insert_node(head, new_node1, 1);
  ck_assert_ptr_eq(new_head, head);
  ck_assert_ptr_eq(node0->next, new_node1);
  ck_assert_ptr_eq(new_node1->next, node1);

  /* Case 2: Insert at end of list (position 3) */
  w1_node *new_node2 = w1_create_node(3);
  new_head = w1_insert_node(new_head, new_node2, 3);
  ck_assert_ptr_eq(node1->next, new_node2);

  /* Case 3: Insert at out of bounds (position 5) */
  new_head = w1_insert_node(new_head, new_node1, 5);
  ck_assert_ptr_eq(new_head, NULL);

  /* Clean-up */
  w1_delete_node(node0);
  w1_delete_node(node1);
  w1_delete_node(new_node1);
  w1_delete_node(new_node2);
  
}
END_TEST

START_TEST(w1_size_list_test) {
  /* Case 1: Empty list */
  w1_node *head = NULL;
  unsigned size = w1_size_list(head);
  ck_assert_uint_eq(size, 0);

  /* Case 2: List of size 2 */
  w1_node *node0 = w1_create_node(0);
  w1_node *node1 = w1_create_node(1);
  head = node0; 
  node0->next = node1;
  size = w1_size_list(head);
  ck_assert_uint_eq(size, 2);
}
END_TEST

START_TEST(w1_print_test) {
  Node *root = newNode(1);
  root->left = newNode(2);
  root->right = newNode(3);
  root->left->left = newNode(4);
  root->left->right = newNode(5);

  /* Case 1: Post-order traversal */
  char expected1[11] = "4 5 2 3 1 ";
  FILE *fp1 = tmpfile();
  w1_print_post_order(root, fp1);
  rewind(fp1);
  char observed1[11];
  fgets(observed1, sizeof(observed1), fp1);
  ck_assert_str_eq(observed1, expected1);

  /* Case 2: Pre-order traversal */
  char expected2[11] = "1 2 4 5 3 ";
  FILE *fp2 = tmpfile();
  w1_print_pre_order(root, fp2);
  rewind(fp2);
  char observed2[11];
  fgets(observed2, sizeof(observed2), fp2);
  ck_assert_str_eq(observed2, expected2);

  /* Case 3: In-order traversal */
  char expected3[11] = "4 2 5 1 3 ";
  FILE *fp3 = tmpfile();
  w1_print_in_order(root, fp3);
  rewind(fp3);
  char observed3[11];
  fgets(observed3, sizeof(observed3), fp3);
  ck_assert_str_eq(observed3, expected3);
}
END_TEST

int main()
{
  Suite* s = suite_create("Week 01 tests");
  TCase *tc1 = tcase_create("basic tests");
  suite_add_tcase(s, tc1);

  /* Add your own tests here */
  tcase_add_test(tc1, example_test);
  tcase_add_test(tc1, example_test2);
  tcase_add_test(tc1, w1_strcmp_test);
  tcase_add_test(tc1, w1_insert_node_test);
  tcase_add_test(tc1, w1_size_list_test);
  tcase_add_test(tc1, w1_print_test);

  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);

  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
