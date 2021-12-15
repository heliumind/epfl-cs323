# Lab 2: Locking and channels

## General information

Release date: 26/10/2021

Due date: 16/11/2021 23:59 CET

We provide you starter code in the `provided/` directory. *Do not edit* any
files within this directory. Copy the contents of this folder into a new
folder: `done/` and make changes there.
*Only* files found in `done/` will be considered for grading. The final state
of the main repository branch at the
deadline will be considered for grading. In addition, we ask you to commit
your code regularly. Having regular commits helps you debug your code by
comparing version across time and allows you to revert to past versions if
needed. A clear progress history will also help your case if you encounter
submission problems ("My internet got cut-off five minutes before the
deadline") or in litigation.

**Important note**: For this lab, the user-level thread library was extended to support multi-threading and real parallelism, i.e., user-level threads are now multiplexed on top of a pool of real hardware threads, therefore allowing several CPU cores to execute different user-level threads at the same time.
We adopted an *N* to *M* mapping, i.e., *N* user-level threads execute on top of *M* real threads.


Steps:
1. spinlock implementation in `spinlock.c` and `spinlock.h`
2. mutex implementation in `mutex.c` and `mutex.h`
3. channel implementation in `channel.c` and `channel.h`
4. Uncomment locks in `execute_commands` in file `linked_list.c`

In this lab, you will be in charge of implementating spinlocks, mutexes, and channels, according to the interfaces defined in the headers above.
We will first implement simple spinlocks, that we will then use to implement blocking mutual exclusion via mutexes.
Finally, we will implement an inter-thread communication mechanism in the form of channels, using our mutexes.

The `utils.h` and `utils.c` files define useful functions that you can use to implement your code and test it.
The `get_current_thread()` function returns the `thread_info_t` pointer to the current thread. A thread that is currently executing is, by default, not in any list, i.e., `thread->prev == thread->next == NULL`.
The `tsafe_unblock_thread` function allows to reschedule a thread that is currently blocked. Be careful, however, that once this function is called, you no longer have exclusive access to the thread structure, i.e., it might be concurrently accessed by other threads.
The `cond_wait` function is similar to the one seen in class. It allows to block a thread and release the lock (either a spinlock or a mutex) atomically once the thread is blocked.
*Be careful, however, as it does not re-acquire a lock before returning!*

You will find more information on the `__sync_*` builtin functions for your version of GCC at [`https://gcc.gnu.org/onlinedocs/gcc-7.4.0/gcc/_005f_005fsync-Builtins.html`](https://gcc.gnu.org/onlinedocs/gcc-7.4.0/gcc/_005f_005fsync-Builtins.html).

## Introduction

In this homework you will be given a linked list implementation, which is NOT thread safe.

Your job is to implement the locking primitives and mechanisms to add thread safety to the linked list. 

The function `execute_commands` in file `linked_list.c` takes commands via one
channel, executes those commands on the global linked list, and returns the
result of those commands via another channel. 

You can find an example of how this works in file `main.c`, where there is a
setup very similar to our tests. To run it, uncomment the line creating the
thread running `try_list` inside the `main` function, and comment the
`try_locks` thread line.
If you then `make main` and run it (`LD_LIBRARY_PATH=. ./main`), you will see that the
result of the last command are not consistent: this is because the linked list is not
thread safe, and there are data races happening. 

You will implement spinlocks, mutexes, and finally channels - after all this, 
you will be able to make the linked list thread safe and see consistent results when
running the above test.


## Spinlock

The `spinlock_t` type is defined in `locks.h`. 
This file also defines the `LOCKED` and `UNLOCKED` values that **must** be used by your code.
The spinlock API is defined in `spinlock.h` and has three functions:

1. `spinlock_init`: Takes a pointer to a spinlock and initializes it, i.e., sets its value to `UNLOCKED`.
2. `spinlock_lock`: Allows to lock a spinlock. As seen in class, you will use the `__sync_val_compare_and_swap` function that allows for a given target to exchange the expected value with the new one if and only if the actual target value is equal to the expected one. The atomic intrinsic returns the actual value read at target.
3. `spinlock_unlock`: unlocks a spinlock. Once again, you can make it atomic by using `__sync_val_compare_and_swap`.

> Note: Trying to unlock a spinlock which is already unlocked should terminate the program. You can use an `assert(some condition)` statement to implement this.
  Beware of potential data-races: implementations which first-check the value, then change it with a separate statement might encounter a TOCTOU (Time-of-check-to-time-of-use) bug.

Look at the `increment_global_spinlock` function in `main.c`. Multiple threads are created by `try_locks` each running this function. The operation in used to increment the global variable is not atomic, so the function uses a spinlock to make sure that only one thread is allowed to increment at a time. Test this!

By running main before you implement the spinlock, you will see that the final value of the global variable is different from the expected value. When you have correctly implemented the spinlock, the final value will match the expected value on each run.

*Aside: Even with an incorrect spinlock, you will see that the expected value of the global matches the final value on some runs. Why do you think this happens? How many cores does your virtual-machine have? Try changing `NITERS` to 1000. What happens now?*

We do not expect you to make these spinlocks fair or highly efficient.
The lock function should, as seen in class, use a while loop to try and set the lock value to `LOCKED`, expecting `UNLOCKED`.
Write tests for your spinlocks, making sure that they work as intended.
They will be the fundational mechanism for the rest of this lab.
While high-efficiency is not a goal, we do expect them to work well enough so that tests do not time-out.

## Mutexes

Next, we will use the spinlocks implemented above to provide mutual exclusion on a `mutex_t` state.
While a spinlock does not deschedule a thread (it just makes it spin until it acquires the lock), mutexes allow to block until the mutex is acquired, therefore allowing the CPU core to perform more useful work than simply spinning.

Once again, we have three functions to implement, for which prototypes are defined in `mutex.h`:
1. `mutex_init`: Initializes a mutex. A mutex contains a spinlock, an owner `tid_t`, initialized to `DEFAULT_TARGET`, and a `thread_list_t` blocked that must be initialized properly (see comments in `mutex.h`).
2. `mutex_lock`: Locks a mutex. *The mutex's spinlock is used for mutual exclusion on the mutex metadata/state.* Once the state is locked, you should try to atomically set the value of owner to the current thread's tid, expecting `DEFAULT_TARGET` as an initial value. For this, use `__sync_val_compare_and_swap` or `__sync_bool_compare_and_swap`. If that fails, it means that the mutex is held by someone else. You therefore need to add yourself to the mutex blocked list, and call `cond_wait` to atomically block and release the spinlock. **YOU DO NOT NEED TO TOUCH THE CURRENT THREAD'S STATE**, this is handled by `cond_wait`. Remember to read the comment for `cond_wait`, this function does not reacquire the lock when it returns, so be careful about the lines of code that you place after it.
3. `mutex_unlock`: Unlocks a mutex. Once again, the spinlock can be used to ensure mutual exclusion on the mutex state. If the list of blocked threads is empty, simply atomically set the value of owner to `DEFAULT_TARGET`. If this is not the case, you need to pop the head of the blocked list, change the owner to this thread's tid, and call `tsafe_unblock_thread`. This function simply fixes the state of the thread before rescheduling it (you should go and read the function's code, it might be helpful later on). 

> Note: Trying to unlock a mutex which is already unlocked should terminate the program. You can use an `assert(some condition)` statement to implement this.

Look at the function `test_global_mutex` in `main.c`. It contains a critical section and tracks how many threads are in the critical section by atomically incrementing a global variable on entry, and decrementing it on exit. Within the critical section, the threads busy-wait by incrementing a local variable to simulate the time spent doing useful work. With proper implementation of mutex, only one thread should be in the critical section at a time. If two or more threads enter the critical section, the value read by `__sync_fetch_and_add` can be non-zero, and the thread will print an error message. Run `main` and see if your implementation works.

*Note: Your mutex implementation also needs to track and wake waiting threads. This function is not an exhaustive test of mutex behavior.*

Mutex implementation is slightly trickier as it requires to block/unblock threads.
As mentioned above, we provide you with all the functions that handle the state of threads.
As a result, if your code explicitly modifies any thread's state, you are doing something wrong.

## Channels

In this part, we will use our mutexes to implement channels, according to the `channel_t` type. 
Channels can be used to send and receive information between two threads in a synchronous manner, i.e., a send (receive) operation will block until another thread performs the corresponding receive (send).
It is a producer/consumer setup in which a value produced by one thread can be consumed at most once, i.e., it can be received only by one consumer. 
A new attribute, `channel_buffer`, was added to `thread_info_t` in order to help you.
Once again, you have three functions to implement, for which prototypes are defined in `channel.h`:

1. `channel_init`: initializes a channel's state.
2. `channel_send`: send some value on a channel.
3. `channel_receive`: receive a value on a channel.

More details about the implementation of these functions can be found in `channel.h`.

Once again, we have set up an example in `main.c`. The `channel_sender` creates an alphabetic string and sends it to `channel_receiver`. The `try_locks` thread checks if the string was sent successfully. 

## A practical use case

*Note: This final part can only be completed after spinlocks, mutexes and channels are completed and working*.

In this part, we will go over a real-world use case of locks.

You will be provided an implementation of a linked list in the files `linked_list.c` and `linked_list.h`.
The implementation is NOT thread safe.

The function `execute_commands` takes commands via one channel, executes those commands on the global linked list,
and returns the result of those commands via another channel. 

If you look at the implementation of `execute_commands`, you will see that there is some locking mechanism commented away. 
Your job is to simply uncomment away those lines to make modifications to the linked list thread-safe.

In our tests, the function `execute_commands` will get called by many threads at once, so make sure that your mutex and spinlock
implementation is working correctly, otherwise you risk performing operations in the wrong order and corrupting the linked list.

The messages received in the channel will be of format `struct command`, which will have three fields:
 - `op`: the type of operation. It is an enum and can have values INSERT, DELETE, or GET.
 - `key`: the key that identifies the element to do the above operation on. It is only relevant
	for DELETE and GET operations.
 - `value`: the contents of the element that is going to be inserted in the list. Only relevant for INSERT operations.

The type of operations are the following:
 - `INSERT`: append an element identified by `key` and containing value `value` to the head of the list.
 - `DELETE`: remove the element identified by `key` from the list.
 - `GET`: find the element identified by `key` and return its value. Return NULL in case it is not found in the list. 

*Bonus question*: What happens if you substitute mutexes with spinlocks in `execute_commands`?  Why? 

*Bonus question*: Our tests expect the result of each command to be sent exactly in the same order as the commands were sent. Is there a different place where you can move the lock/unlock mechanism inside `execute_commands`, or not? 
