/**
 * @brief Implementation of the futex function.
 *
 * @author Adrien Ghosn
 */
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/time.h>
#include<linux/futex.h>
#include <unistd.h>
#include "futex.h"

int futex(int* uaddr, int futexop, int val) {
  return syscall(SYS_futex, uaddr, futexop, val, NULL, NULL, 0); 
}

