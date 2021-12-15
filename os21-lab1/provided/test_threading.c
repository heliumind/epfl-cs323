/**
 * @file test_threading.c
 * @brief Unit-test suite for week 3
 *
 * @author Mark Sutherland
 */
#include <check.h>
#include <stdlib.h> 
#include "sched_policy.h"
int global_executed = 0;

void* thread_execute(void* arg) {
    global_executed = 1;
    return NULL;
}
START_TEST(thread_executed_test) {
  // This test spawn a thread which increment a global variable. In case
  // thread create does not correctly setup the stack or switch to the new
  // thread the global variable will not be incremented and the test will fail.
  global_executed = 0;
  clean_up_scheduler();
  initialize_scheduler(l1_round_robin_policy);
  l1_tid execute;
  l1_thread_create(&execute, thread_execute, NULL);
  schedule();
  // no race condition as schedule only exit when it is the last running thread 
  ck_assert_msg(global_executed == 1, "Thread should execute the passed routine.");
}
END_TEST

int main(int argc, char **argv)
{
    Suite* s = suite_create("Stack Library Tests");
    TCase *tc1 = tcase_create("basic"); 
    suite_add_tcase(s,tc1);

    /* TODO: Write your own tests */
    tcase_add_test(tc1, thread_executed_test);

    SRunner *sr = srunner_create(s); 
    srunner_run_all(sr, CK_VERBOSE); 

    int number_failed = srunner_ntests_failed(sr); 
    srunner_free(sr); 

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE; 
}
