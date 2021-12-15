/**
 * @file main.c
 * @brief Sample application using functions from week 2
 *
 * @author Ahmad Hazimeh
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "week01.h"
#include "week02.h"

void verify_fork()
{
    fprintf(stdout, "Parent PID: %d, Own PID: %d\n", getppid(), getpid());
    fflush(stdout);
}

/* `thread_id` is stored in the global memory of the process.
 * When the parent process forks, a copy of global memory is inherited by
 * the child; the child retains the initial value after the fork, but any
 * further writes in the child only affect the child's copy.
 */
static int thread_id = 0;

void * verify_thread(void *arg)
{
    static pthread_mutex_t iomux = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&iomux);
    fprintf(stdout, "Thread ID: %d, Process ID: %d\n", thread_id, getpid());
    fflush(stdout);
    ++thread_id;
    pthread_mutex_unlock(&iomux);
    return NULL;
}

int main(int argc, char **argv)
{
    /***** Task 1: w2_bork() *****/
    printf("Simple fork examples:\n");

    /* This call should print lines resembling the following output:
     * Parent PID: A, Own PID: B
     */
    w2_bork(1, verify_fork);
    printf("\n");

    /* This call should print lines resembling the following output:
     * Parent PID: A, Own PID: B
     * Parent PID: A, Own PID: C
     * Parent PID: A, Own PID: D
     * Parent PID: A, Own PID: E
     * Parent PID: A, Own PID: F
     */
    w2_bork(5, verify_fork);
    printf("\n");

    /***** Task 2: w2_fork() *****/
    printf("Fork tree examples:\n");

    /* This call should print lines resembling the following output:
     * Parent PID: A, Own PID: B
     * Parent PID: B, Own PID: C
     */
    bool fork_tree0[] = {1, 1, 0, 0, 0, 0, 0};
    w2_fork(fork_tree0, 0, verify_fork);
    printf("\n");

    /* This call should print lines resembling the following output:
     * Parent PID: A, Own PID: B
     * Parent PID: B, Own PID: C
     * Parent PID: C, Own PID: D
     * Parent PID: A, Own PID: E
     * Parent PID: E, Own PID: F
     * Parent PID: A, Own PID: G
     */
    /* This is the skeleton tree given in the description of w2_fork(). */
    bool fork_tree1[] = {1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    w2_fork(fork_tree1, 0, verify_fork);
    printf("\n");

    /* This call should print lines resembling the following output:
     * Parent PID: A, Own PID: B
     * Parent PID: B, Own PID: C
     * Parent PID: C, Own PID: D
     * Parent PID: D, Own PID: E
     * Parent PID: C, Own PID: F
     * Parent PID: B, Own PID: G
     * Parent PID: G, Own PID: H
     * Parent PID: B, Own PID: I
     */
    bool fork_tree2[] = {1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    w2_fork(fork_tree2, 0, verify_fork);
    printf("\n");

    /***** Task 3: w2_clone() *****/
    printf("Clone tree examples:\n");

    /* This call should print lines resembling the following output:
     * Thread ID: 0, Process ID: A
     * Parent PID: A, Own PID: B
     * Thread ID: 1, Process ID: A
     * Thread ID: 2, Process ID: A
     * Thread ID: 3, Process ID: A
     * Parent PID: A, Own PID: C
     */
    struct pnode clone_tree0[] = {
        {.must_fork=1, .num_threads=1},
        {.must_fork=0, .num_threads=0},
        {.must_fork=1, .num_threads=3},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0}
    };
    thread_id = 0; // reset to 0 before tests
    w2_clone(clone_tree0, 0, verify_thread, verify_fork);
    printf("\n");

    /* This call should print lines resembling the following output:
     * Thread ID: 0, Process ID: A
     * Thread ID: 1, Process ID: A
     * Thread ID: 2, Process ID: A
     * Parent PID: A, Own PID: B
     * Thread ID: 3, Process ID: B
     * Thread ID: 4, Process ID: B
     * Parent PID: B, Own PID: C
     * Parent PID: C, Own PID: D
     * Thread ID: 3, Process ID: A
     * Thread ID: 4, Process ID: A
     * Thread ID: 5, Process ID: A
     * Thread ID: 6, Process ID: A
     * Parent PID: A, Own PID: E
     * Thread ID: 7, Process ID: E
     * Parent PID: E, Own PID: F
     * Thread ID: 7, Process ID: A
     * Thread ID: 8, Process ID: A
     * Thread ID: 9, Process ID: A
     * Parent PID: A, Own PID: G
     */
    /* This is the skeleton tree given in the description of w2_clone(),
     * with the thread spawn points positioned as illustrated.
     */
    struct pnode clone_tree1[] = {
        {.must_fork=1, .num_threads=3},
        {.must_fork=1, .num_threads=2},
        {.must_fork=1, .num_threads=4},
        {.must_fork=1, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=1, .num_threads=1},
        {.must_fork=1, .num_threads=3},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
    };
    thread_id = 0; // reset to 0 before tests
    w2_clone(clone_tree1, 0, verify_thread, verify_fork);
    printf("\n");

    /* This call should print lines resembling the following output:
     * Thread ID: 0, Process ID: A
     * Parent PID: A, Own PID: B
     * Thread ID: 1, Process ID: B
     * Thread ID: 2, Process ID: B
     * Thread ID: 3, Process ID: B
     * Parent PID: B, Own PID: C
     * Thread ID: 4, Process ID: B
     * Parent PID: B, Own PID: D
     * Thread ID: 5, Process ID: D
     * Thread ID: 6, Process ID: D
     * Parent PID: D, Own PID: E
     * Thread ID: 5, Process ID: B
     * Parent PID: B, Own PID: F
     * Thread ID: 1, Process ID: A
     * Parent PID: A, Own PID: G
     * Thread ID: 2, Process ID: G
     * Thread ID: 3, Process ID: G
     * Thread ID: 4, Process ID: G
     * Parent PID: G, Own PID: H
     * Parent PID: H, Own PID: I
     * Thread ID: 5, Process ID: G
     * Parent PID: G, Own PID: J
     * Parent PID: A, Own PID: K
     * Thread ID: 2, Process ID: K
     * Thread ID: 3, Process ID: K
     * Thread ID: 4, Process ID: K
     * Parent PID: K, Own PID: L
     * Thread ID: 2, Process ID: A
     * Thread ID: 3, Process ID: A
     * Parent PID: A, Own PID: M
     */
    struct pnode clone_tree2[] = {
        {.must_fork=1, .num_threads=1},
        {.must_fork=1, .num_threads=3},
        {.must_fork=1, .num_threads=1},
        {.must_fork=0, .num_threads=0},
        {.must_fork=1, .num_threads=1},
        {.must_fork=1, .num_threads=3},
        {.must_fork=1, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=1, .num_threads=2},
        {.must_fork=1, .num_threads=1},
        {.must_fork=1, .num_threads=0},
        {.must_fork=1, .num_threads=1},
        {.must_fork=1, .num_threads=3},
        {.must_fork=1, .num_threads=2},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0},
        {.must_fork=0, .num_threads=0}
    };
    thread_id = 0; // reset to 0 before tests
    w2_clone(clone_tree2, 0, verify_thread, verify_fork);
    printf("\n");

    return 0;
}