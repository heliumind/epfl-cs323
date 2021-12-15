/**
 * @brief API for the global scheduler state
 *
 * @author Adrien Ghosn
 */

#include <pthread.h>
#include "schedule.h"

/*Flag signaling that the execution is over.*/
#define IS_OVER (1<<30)

/**
 * @brief The global state of the scheduler.
 * We have centralized queues to make priority scheduling easier.
 */
typedef struct {
  int sleep_count;    /* Number of sys threads sleeping*/
  uint64_t total_sys; /* Number of sys threads*/
  int _start;         /* Futex used to start the threads */

  tid_t next_tid;                                /* Global tid */
  sched_policy select_next;                      /* Scheduler policy*/
  thread_list_t thread_arrays[NUM_THREAD_STATES];/* Lists for thread in different states */
  pthread_mutex_t list_locks[NUM_THREAD_STATES]; /* Locks for the thread lists */

  /* Global knowledge about threads existing */
  pthread_mutex_t exists_lock;
  thread_list_t exists;
} scheduler_state_t;


/**
 * @brief initiliazes the global_state variable.
 * @warning NOT THREAD SAFE, SHOULD BE CALLED ONCE.
 */
void initialize_scheduler_state(sched_policy policy, int nb_sys_threads);

/**
 * @brief Frees the scheduler state.
 *
 * @warning not thread safe!
 */
void destroy_scheduler_state();

/**
 * @brief removes thread from exists list.
 *
 * THREAD SAFE
 */
thread_info_t* unregister_thread(thread_info_t* t); 

/**
 * @brief checks if a thread exists. 
 *
 * NOT THREAD SAFE
 */
thread_info_t* does_thread_exists(tid_t tid);

/**
 * @brief registers thread in special exists list.
 *
 * THREAD SAFE
 */
void register_thread(thread_info_t* t);

/**
 * @brief returns a pointer to the scheduler state variable.
 */
scheduler_state_t* get_scheduler_state();

/**
 * @brief Generate unique TIDs by incrementing scheduler variable
 *
 * THREAD SAFE
 */
tid_t get_uniq_tid();

/**
 * @brief selects the next thread to run, removes it from the scheduling queue,
 * and returns it.
 *
 * @warn THREAD SAFE IF AND ONLY IF sched_policy is thread safe.
 */
thread_info_t* select_next(thread_info_t* cur, thread_info_t* target);

/**
 * @brief enqueues t inside the thread state.
 *
 * THREAD SAFE
 */
void tsafe_enqueue_thread(thread_info_t* t, thread_state_t s);

/**
 * @brief remove the thread t from the list that corresponds to state s.i
 *
 * THREAD_SAFE
 */
void tsafe_remove_thread(thread_info_t* t, thread_state_t s);

/**
* @brief Locks list of state s.
*
* THREAD SAFE
*/
void lock_list(thread_state_t s);

/**
 * @brief Unlocks list of state s.
 *
 * THREAD SAFE
 */
void unlock_list(thread_state_t s);

/**
 * @brief finds a thread in a list of state s and removes it.
 *
 * THREAD SAFE
 */
thread_info_t* tsafe_find_and_remove_thread(tid_t target, thread_state_t s);

/**
 * @brief locks ZOMBIE and then JOINING
 *
 * THREAD SAFE
 */
void lock_zombie_joining();

/**
 * @brief unlocks JOINING and the ZOMBIE
 *
 * THREAD SAFE
 */
void unlock_zombie_joining();

/**
 * @brief Locks all thread lists in ZOMBIE, JOINING, RUNNABLE order.
 */
void lock_all_lists();

/**
 * @brief Unlocks all lists in RUNNABLE, JOINING, ZOMBIE order.
 */
void unlock_all_lists();

