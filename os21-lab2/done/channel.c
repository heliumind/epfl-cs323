/**
 * @file channel.c
 *
 * @brief Implementation of channels.
 *
 * @author Adrien Ghosn
 *
 */
#include <assert.h>
#include "utils.h"
#include "mutex.h"
#include "channel.h"
#include "sys_thread.h"
#include "scheduler_state.h"

void channel_init(channel_t* chan) {
  /* TODO: Implement */
  mutex_init(&chan->mu);
  thread_list_init(&chan->blocked_recv);
  thread_list_init(&chan->blocked_send);
}

void channel_send(channel_t* chan, void* value) {
  /* TODO: Implement */
  mutex_lock(&chan->mu);
  if (!thread_list_is_empty(&chan->blocked_recv)) {
    thread_info_t *recv_thread = thread_list_pop(&chan->blocked_recv);
    recv_thread->channel_buffer = value;
    tsafe_unblock_thread(recv_thread);
    mutex_unlock(&chan->mu);
  } else {
    thread_info_t *send_thread = get_current_thread();
    send_thread->channel_buffer = value;
    thread_list_add(&chan->blocked_send, send_thread);
    cond_wait(&chan->mu, MUTEX);
    assert(send_thread->channel_buffer == NULL);
  }
}

void* channel_receive(channel_t* chan) {
  /* TODO: Implement */
  void *value;
  mutex_lock(&chan->mu);
  if (!thread_list_is_empty(&chan->blocked_send)) {
    thread_info_t *send_thread = thread_list_pop(&chan->blocked_send);
    value = send_thread->channel_buffer;
    send_thread->channel_buffer = NULL;
    tsafe_unblock_thread(send_thread);
    mutex_unlock(&chan->mu);
    return value;
  } else {
    thread_info_t *recv_thread = get_current_thread();
    thread_list_add(&chan->blocked_recv, recv_thread);
    cond_wait(&chan->mu, MUTEX);
    value = recv_thread->channel_buffer;
    recv_thread->channel_buffer = NULL;
    return value;
  }
  return NULL;
}
