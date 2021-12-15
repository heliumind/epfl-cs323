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
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "week02.h"

void w2_bork(unsigned int n, void (*verify)(void))
{
    if (n == 0 /* TODO: add recursion base case */)
        return;

    pid_t cpid = fork();
    if (cpid < 0 /* TODO: add fork() failure condition */) {
        fprintf(stderr, "fork(): errno %d %s\n", errno, strerror(errno));
        return;
    }
    if (cpid == 0 /* TODO: check if it's the child process */) {
        /* TODO: call the `verify` function (check if NULL first) */
        /* TODO: terminate child */
        if (verify != NULL)
            verify();
    } else {
        /* This is only executed by the parent process */
        /* TODO: wait for child to exit */
        /* TODO: add recursive function call to spawn remaining children */
        wait(NULL);
        w2_bork(n - 1, verify);
    }
}

void w2_fork(const bool *root, int index, void (*verify)(void))
{
    /* TODO: Replace with your own code */
    /* Corner cases: invalid inputs */
    if (root == NULL || index < 0) 
        return;

    bool isNode = root[index];

    if (isNode) {
        pid_t cpid = fork();

        if (cpid < 0) { // fork() failure
            fprintf(stderr, "fork(): errno %d %s\n", errno, strerror(errno));
        }
        else if (cpid == 0) { // child process
            if (verify != NULL)
                verify();
            w2_fork(root, 2 * index + 1, verify);
            exit(0);
        }
        else { // parent process
            wait(NULL);
            w2_fork(root, 2 * index + 2, verify);
        }
    }
}

void w2_clone(const struct pnode *root, int index, void * (*verify_thread)(void *), void (*verify_fork)(void))
{
    /* TODO: Replace with your own code */
    /* Corner cases: invalid inputs */
    if (root == NULL || index < 0)
        return;
    
    /* Unpack pnode */
    struct pnode fork_node = root[index];
    int thread_count = fork_node.num_threads;
    
    if (fork_node.must_fork) {

        /* Check if thread_count is valid */
        if (thread_count < 0) {
            fprintf(stderr, "Cannot create threads. Process exited.");
            exit(1);
            return;
        }

        pthread_t thread_ids[thread_count];
        /* Spawn threads */
        if (verify_thread != NULL) {
            /* Create thread */
            for (int i = 0; i < thread_count; i++) {
                pthread_create(&thread_ids[i], NULL, verify_thread, NULL);
            }
            /* Join each thread */ 
            for (int i = 0; i < thread_count; i++)
            {
                pthread_join(thread_ids[i], NULL);
            }
        }

        /* Fork */
        pid_t cpid = fork();

        if (cpid < 0) { // fork() failure
            fprintf(stderr, "fork(): errno %d %s\n", errno, strerror(errno));
        }
        else if (cpid == 0) { // child process
            if (verify_fork != NULL)
                verify_fork();
            w2_clone(root, 2 * index + 1, verify_thread, verify_fork);
            exit(0);
        }
        else { // parent process
            wait(NULL);
            w2_clone(root, 2 * index + 2, verify_thread, verify_fork);
        }
    }
}
