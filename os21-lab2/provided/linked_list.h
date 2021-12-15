#include "channel.h"

typedef struct node_t node_t;
typedef struct node_t {
  int data;
  int key;
  node_t *next;
} node_t;

/**
 * @brief Displays the contents of the list.
 *
 */
void list_print();

/**
 * @brief Insert node_t with given key and contents "data"
 * Inserts a new node_t identified by "key" and with contents "data",
 * to the head of the global linked list.
 */
void list_insert(int key, int data);

/**
 * @brief Delete the first node_t of the list.
 */
node_t* list_delete_first();

/**
 * @brief Return the node_t identified by the given "key".
 * If no node_t with the given key is found in the list, 
 * returns NULL.
 */
node_t* list_find(int key);


/**
 * @brief Deletes the node_t identified by the given "key" and returns it.
 * If no node_t with the given key is found in the list, 
 * returns NULL.
 */
node_t* list_delete(int key);


typedef struct {
  enum operation {INSERT, DELETE, GET} op;
  int key;
  int val;
} command;

typedef struct {
  channel_t* to_recv;
  channel_t* to_send;
} bichannels_t;

mutex_t list_lock;

void* execute_commands(void* bichan);
void execute_commands_init();
