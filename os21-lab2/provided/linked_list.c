#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

 node_t *head = NULL;
 node_t *current = NULL;

//display the list
void list_print() {
    node_t *ptr = head;
   printf("\n[ ");
    
   //start from the beginning
   while(ptr != NULL) {
      printf("(%d,%d) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
    
   printf(" ]");
}

//insert link at the first location
void list_insert(int key, int data) {
   //create a link
    node_t *link = ( node_t*) malloc(sizeof( node_t));
    
   link->key = key;
   link->data = data;
    
   //point it to old first node_t
   link->next = head;
    
   //point first to new first node_t
   head = link;
}

//delete first item
 node_t* list_delete_first() {

   //save reference to first link
    node_t *tempLink = head;
    
   //mark next to first link as first 
   head = head->next;
    
   //return the deleted link
   return tempLink;
}

int length() {
   int length = 0;
    node_t *current;
    
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
    
   return length;
}

//find a link with given key
 node_t* list_find(int key) {

   //start from the first link
    node_t* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {
    
      //if it is last node_t
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
    
   //if data found, return the current Link
   return current;
}

//delete a link with given key
 node_t* list_delete(int key) {

   //start from the first link
    node_t* current = head;
    node_t* previous = NULL;
    
   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {

      //if it is last node_t
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;
      }
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      previous->next = current->next;
   }    
    
   return current;
}

void sort() {

   int i, j, k, tempKey, tempData;
    node_t *current;
    node_t *next;
    
   int size = length();
   k = size ;
    
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = head;
      next = head->next;
        
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data > next->data ) {
            tempData = current->data;
            current->data = next->data;
            next->data = tempData;

            tempKey = current->key;
            current->key = next->key;
            next->key = tempKey;
         }
            
         current = current->next;
         next = next->next;
      }
   }   
}

void execute_commands_init() {
  mutex_init(&list_lock);
}

void* execute_commands(void* bichan) {
  // TODO: uncomment this!
  //mutex_lock(&list_lock);

  bichannels_t* chan = (bichannels_t*) bichan;
  command* recv_command = (command*) channel_receive(chan->to_recv);

  node_t* n;
  unsigned long retval = 0;
  switch (recv_command->op) {
    case INSERT:
      list_insert(recv_command->key, recv_command->val);
      break;
    case DELETE:
      list_delete(recv_command->key);
      break;
    case GET:
      n = list_find(recv_command->key);
      if (n) {
         retval = n->data;
      }
      break;
  }

  channel_send(chan->to_send, (void *)retval);

  // TODO: uncomment this!
  //mutex_unlock(&list_lock);
  return NULL;
}
