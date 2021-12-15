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

int main()
{
  Suite* s = suite_create("Week 01 tests");
  TCase *tc1 = tcase_create("basic tests");
  suite_add_tcase(s, tc1);

  /* Add your own tests here */
  tcase_add_test(tc1, example_test);
  tcase_add_test(tc1, example_test2);

  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);

  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
