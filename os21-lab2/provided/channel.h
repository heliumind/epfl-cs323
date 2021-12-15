/**
 * @file channel.h
 *
 * @brief API for the channels. 
 *
 * @author Adrien Ghosn
 *
 */
#pragma once
#include "mutex.h"
#include "thread_list.h"

typedef struct {
  mutex_t mu;                 /* Mutex for mutual exclusion on the channel. */
  thread_list_t blocked_recv; /* List of blocked threads for a receive. */
  thread_list_t blocked_send; /* List of blocked threads for a send. */
} channel_t;

/**
 * @brief Initializes a channel.
 *
 * This function has to call mutex_init on the channel's mutex, and initialize
 * both the blocked_recv and blocked_send lists using thread_list_init.
 *
 * DOES NOT HAVE TO BE THREAD SAFE.
 */
void channel_init(channel_t* chan);

/**
 * @brief Allows to do a blocking send of value on channel chan.
 * 
 * The function must lock chan->mu to ensure mutual exclusion on the chan state.
 * If the blocked_recv queue is not empty, pop the list, make the unblocked's 
 * thread channel_buffer point to value, and use tsafe_unblock_thread to reschedule it.
 * Do not forget to unlock the mutex!
 * If the blocked_recv queue is empty, the current thread must set its channel_buffer
 * to point to value and enqueue itself inside the blocked_send list.
 * Use the cond_wait function to atomically block itself and release the mutex.
 * If your implementation is correct, the current thread's channel_buffer must be null
 * at that point.
 *
 * MUST BE THREAD SAFE
 */
void channel_send(channel_t* chan, void* value);

/**
 * @brief Allows to receive a value on a channel chan.
 *
 * The function must lock chan->mu to ensure mutual exclusion on the channel.
 * If the blocked_send list is not empty, pop the list, get the unblocked thread's
 * value from its channel_buffer, set the thread's channel_buffer to NULL,
 * (as described in channel_send final sentence) and use tsafe_unblock_thread to 
 * reschedule it. You can then return the value that you read.
 * Don't forget to unlock the mutex!
 * If the queue is empty, add yourself to the blocked_recv queue,
 * use cond_wait to atomically block and release the mutex.
 * Once you return, the value read should be inside channel_buffer. Read it,
 * save it, reset channel_buffer to null, and return the value.
 */
void* channel_receive(channel_t* chan);
