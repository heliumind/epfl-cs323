/**
 * @brief tests for the spinlock.
 *
 * @Author Adrien Ghosn
 *
 */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <check.h>
#include "scheduler_state.h"
#include "sys_thread.h"
#include "thread.h"
#include "spinlock.h"
#include "mutex.h"
#include "channel.h"
#include "utils.h"
#include "sched_policy.h"
#include "assert.h"
#include "linked_list.h"

int main(void) {

  Suite* s = suite_create("Locking/IPC lab");
 
  SRunner *sr = srunner_create(s); 
  srunner_run_all(sr, CK_VERBOSE); 

  int number_failed = srunner_ntests_failed(sr); 
  srunner_free(sr); 
  return (number_failed ==0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


