/**
 * @file test_malloc.c
 * @brief Unit-test suite for week 5
 *
 * @author Atri Bhattacharyya, Ahmad Hazimeh
 */
#include <check.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stddef.h>
#include "malloc.h"

void *(*l1_malloc)(size_t) = libc_malloc;
l1_error (*l1_free)(void *) = libc_free;
void (*l1_init)(void) = NULL;
void (*l1_deinit)(void) = NULL;

START_TEST(chunk_malloc_test_1) {
    /* This will test the chunk allocator */
  l1_init = l1_chunk_init;
  l1_deinit = l1_chunk_deinit;
  l1_malloc = l1_chunk_malloc;
  l1_free = l1_chunk_free;

  l1_init();
  void *ptr = l1_malloc(0);
  ck_assert_msg(ptr == NULL, "An allocation of size 0 should fail.");
  l1_deinit();
}
END_TEST

/* Test malloc with 0 size returns NULL */
START_TEST(list_malloc_test_1) {
  /* This will test the list allocator */
  l1_init = l1_list_init;
  l1_deinit = l1_list_deinit;
  l1_malloc = l1_list_malloc;
  l1_free = l1_list_free;

  l1_init();
  ck_assert_msg(l1_malloc(0) == NULL, "A malloc of size 0 should return NULL.");
  l1_deinit();
}
END_TEST

START_TEST(magic_equals_test) {
  max_align_t magic_number;
  srand(time(NULL));
  for(unsigned i = 0; i < sizeof(max_align_t); ++i)
    *(((char *)&magic_number) + i) = rand();

  int result1 = magic_equals(&magic_number, &magic_number);
  ck_assert_int_eq(result1, 1);
  max_align_t *corrupt = malloc(sizeof(max_align_t));
  memset(corrupt, 0, sizeof(max_align_t));
  int result2 = magic_equals(corrupt, &magic_number);
  ck_assert_int_eq(result2, 0);
}
END_TEST

int main(int argc, char **argv)
{
  Suite* s = suite_create("Threading lab");
  TCase *tc1 = tcase_create("basic tests"); 
  suite_add_tcase(s,tc1);

  tcase_add_test(tc1, chunk_malloc_test_1);
  tcase_add_test(tc1, list_malloc_test_1);

  /* Add more tests of your own */
  tcase_add_test(tc1, magic_equals_test);

  SRunner *sr = srunner_create(s); 
  srunner_run_all(sr, CK_VERBOSE); 

  int number_failed = srunner_ntests_failed(sr); 
  srunner_free(sr); 

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
