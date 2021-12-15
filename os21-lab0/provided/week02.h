/**
 * @file week02.h
 * @brief Header file for week 2 exercises
 *
 * @author Ahmad Hazimeh
 */
#pragma once
#include <stdbool.h>

/***** Task 1 : Simple Fork */

/**
 * @brief      Forks the parent process `n` times
 *
 * Spawns `n` children of the parent process through fork(). After every fork,
 * the spawned child calls the `verify` function (if not NULL), with no
 * arguments: `verify();`
 *
 * The child then terminates by calling `exit(0);`.
 *
 * Additionally, to avoid problems of data races and concurrency, the parent
 * must wait until its forked child terminates, before spawning the next child.
 * This is done by having the parent issue a call to `wait(NULL);`.
 *
 * Check the manpages of FORK(2) to determine how to differentiate between the
 * parent and the child at runtime.
 *
 * This function can be implemented either sequentially or recursively. However,
 * to pave the way for the upcoming tasks, we've provided a recursive outline of
 * this function, which you need to complete.
 *
 * Two sample test cases and outputs are provided in main.c
 *
 * @param[in]  verify  Pointer to the verification function.
 */
void w2_bork(unsigned int n, void (*verify)(void));


/***** Task 2 : Fork Tree */

/**
 * @brief Forks the parent processes as a tree.
 *
 * Creates a tree of processes using the fork() method. A parent process decides
 * when to fork based on the provided skeleton tree, stored in `root`.
 * The skeleton tree is a full binary tree which determines when a process needs
 * to fork, based on a boolean value stored in its nodes. All internal nodes in
 * the skeleton tree hold a value T, whereas all leaf nodes hold a value F.
 * An example skeleton tree is given here:
 *
 *              (A)                                 (B)
 *
 *              +-+                                 +-+
 *              |T|                                 |0|
 *              +++                                 +++
 *               |                                   |
 *        +------+------+                     +------+------+
 *        |             |                     |             |
 *       +++           +++                   +++           +++
 *       |T|           |T|                   |1|           |0|
 *       +++           +++                   +++           +++
 *        |             |                     |             |
 *    +---+---+     +---+---+             +---+---+     +---+---+
 *    |       |     |       |             |       |     |       |
 *   +++     +++   +++     +++           +++     +++   +++     +++
 *   |T|     |F|   |T|     |T|           |2|     |1|   |4|     |0|
 *   +++     +-+   +++     +++           +++     +-+   +++     +++
 *    |             |       |             |             |       |
 *  +-+-+         +-+-+   +-+-+         +-+-+         +-+-+   +-+-+
 *  |   |         |   |   |   |         |   |         |   |   |   |
 * +++ +++       +++ +++ +++ +++       +++ +++       +++ +++ +++ +++
 * |F| |F|       |F| |F| |F| |F|       |3| |2|       |5| |4| |6| |0|
 * +-+ +-+       +-+ +-+ +-+ +-+       +-+ +-+       +-+ +-+ +-+ +-+
 *
 * In figure (A): at the root, with a value of T, the parent forks: the forked
 * (child) process is assigned the left node, whereas the parent process is now
 * the right node. The child, whose node also holds a value of T, forks again,
 * creating another child at its left, and re-assigning itself to the right
 * node. With a value of T, the newest child also forks, whereas the previous
 * child with a value of F in its node does not fork anymore.
 *
 * To extract a deterministic order in which the forked processes execute, each
 * parent, after forking, issues a call to wait(). By waiting on the forked
 * process, the parent suspends execution until its child terminates. Using the
 * described structure of the skeleton tree, the left node is always evaluated
 * before the right node, since the right node is the parent, and the left node
 * is the child (which executes while the parent is waiting). In the given
 * example: figure (B) shows the PID of every process at every node. Assuming
 * that the PID is incremented by 1 at every fork, and starting from PID 0, the
 * skeleton tree in (A) will produce the PID tree in (B).
 *
 * A better visual representation showing how execution forks is portrayed below.
 * In this diagram, time increases towards the right. As we can see, initially,
 * there is only process 0. At each '+', the process forks and at every 'X', the
 * process exits. Thus, we see process 1 forking to create process 2, and
 * waiting for process 2 to finish before exiting.
 *
 *             3---X
 *              |
 *         2----+---X        5------X
 *          |                 |
 *     1----+---------X  4----+-------X  6------------X
 *      |                 |               |
 * 0----+-----------------+---------------+--------------->
 *
 * After every fork, the forked process must call the provided function `verify`
 * (if not NULL). The function takes no parameters and returns no value. It is
 * simply called by writing: verify();
 * After finishing its task, the forked process must terminate with a call to
 * exit(0).
 *
 * The skeleton tree is stored in the `root` parameter, in its sequential form.
 * In the provided array, for a node at index i:
 * - left child index: 2*i + 1
 * - right child index: 2*i + 2
 * http://www.icodeguru.com/vc/10book/books/book2/algo02a7.htm contains a quick
 * refresher on the sequential representation of a binary tree.
 *
 * The boolean values, T and F, are stored as boolean values, `true` and `false`
 * respectively, in the array `root`.
 *
 * HINT: this function is meant to be called recursively.
 *
 * Three sample test cases and outputs are provided in main.c
 *
 * @param  root    Pointer to an array storing the sequential representation of
 *                 the skeleton tree (binary tree).
 * @param  index   The index of the current node in `root`.
 * @param  verify  Pointer to the verification function.
 */
void w2_fork(const bool *root, int index, void (*verify)(void));


/***** Task 3 : Forks and Threads */

/**
 * @brief      Process tree node
 */
struct pnode {
    bool must_fork;
    int num_threads;
};

/**
 * @brief      Forks a tree of parent processes, each running multiple threads.
 *
 * In this exercise, we will extend Task 1 to allow each process to spawn
 * multiple threads before forking. This simulates a case where a process needs
 * to finish a parallel task quickly before offloading it to a forked process.
 *
 * Re-considering the example of Task 1:
 *
 *                3---X
 *                 |
 *            2----+---X          5------X
 *             |                   |
 *        1--•-+---------X    4--•-+-------X  6------------X
 *         |                   |               |
 * 0-----•-+-----------------•-+-------------•-+--------------->
 *
 * Each dot (•) sign represents the point in time when threads are spawned. All
 * threads are spawned at that point, and all threads must be joined before
 * forking. There is no restriction on the order of threads created or joined.
 * Threads are created using the pthread_create() method, and the thread's
 * creator can wait on the created thread to finish using the pthread_join()
 * method. For proper thread termination and cleanup, the creator must join its
 * spawned threads before it exits.
 *
 * As in Task 1, the skeleton tree is stored in its sequential form in `root`.
 * Every node, of type `struct pnode`, contains a boolean value indicating if
 * the process at that node should fork, and if it should, how many threads it
 * should spawn before forking. If a process does not need to fork, it must not
 * spawn any threads. If a parent process has to fork, then it must spawn
 * `num_threads` threads before forking. Every new thread created must call the
 * provided function `verify_thread` (if not NULL). The function takes a null
 * parameter and returns a null pointer (for use with pthread_create).
 *
 * Additionally, after every fork, as in Task 1, the forked process must call
 * the provided function `verify_fork` (if not NULL). The function takes no
 * parameters and returns no value. It is simply called by writing:
 * verify_fork();
 *
 * HINT: Do not lose reference to created threads, because you need to join them
 *       later to ensure they all finished their tasks.
 *
 * Three sample test cases and outputs are provided in main.c
 *
 * @param  root    Pointer to an array storing the sequential representation of
 *                 the skeleton tree.
 * @param  index   The index of the current node in `root`.
 * @param  verify_thread  Pointer to the thread verification function.
 * @param  verify_fork    Pointer to the fork verification function.
 */
void w2_clone(const struct pnode *root, int index, void * (*verify_thread)(void *), void (*verify_fork)(void));