/**
 * @file week02.c
 * @brief Exercises in C for week 2
 *
 * TODO's:
 * 1. Complete the implementation of w2_bork()
 * 2. Implement w2_fork()
 * 3. Implement w2_clone()
 *
 * @author Ahmad Hazimeh
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "week02.h"

void w2_bork(unsigned int n, void (*verify)(void))
{
    if (0 /* TODO: add recursion base case */)
        return;

    pid_t cpid = fork();
    if (0 /* TODO: add fork() failure condition */) {
        fprintf(stderr, "fork(): errno %d %s\n", errno, strerror(errno));
        return;
    }
    if (0 /* TODO: check if it's the child process */) {
        /* TODO: call the `verify` function (check if NULL first) */
        /* TODO: terminate child */
    } else {
        /* This is only executed by the parent process */
        /* TODO: wait for child to exit */
        /* TODO: add recursive function call to spawn remaining children */
    }
}

void w2_fork(const bool *root, int index, void (*verify)(void))
{
    /* TODO: Replace with your own code */
}

void w2_clone(const struct pnode *root, int index, void * (*verify_thread)(void *), void (*verify_fork)(void))
{
    /* TODO: Replace with your own code */
}
