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
}

void channel_send(channel_t* chan, void* value) {
  /* TODO: Implement */
}

void* channel_receive(channel_t* chan) {
  /* TODO: Implement */
  return NULL;
}
