#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

/* Enum for indices and size of array of node pointers */
typedef enum {Head, Tail, Ends} End;

/* Node structure for doubly-ended queue (deq) */
typedef struct Node {
  struct Node *np[Ends]; // Next/Prev neighbors. [Head] for previous, [Tail] for next
  Data data;             // Data stored in the node
} *Node;

/* Representation structure for doubly-ended queue (deq) */
typedef struct {
  Node ht[Ends]; // [Head] for head node, [Tail] for tail node
  int len;       // Length of the doubly-ended queue
} *Rep;

/* Return the Representation of a Deq */
static Rep rep(Deq q) {
  if (!q) ERROR("zero pointer");
  return (Rep)q;
}

/**
 * Allocates a new node, sets its data field to the given data, and inserts
 * it at either the head or the tail of the deque represented by 'r' depending on the value
 * given for 'e' (Head or Tail). The length of the deque is incremented.
 *
 * @param r    A Rep structure representing the deque.
 * @param e    An End enumeration value indicating where to insert the new node (either Head or Tail).
 * @param d    A Data structure containing the data to be inserted.
 *
 *
 * @return void
 */
static void put(Rep r, End e, Data d) {
   // Check if the deque representation is NULL
  if (r == NULL) {
    ERROR("Attempting to add a node to a non-existent deq");
    return;
  }
  
  //create new node
  Node newNode = (Node)malloc(sizeof(*newNode));

  //check if malloc failed
  if (!newNode) {
    ERROR("Failed memory allocation for new node");
    return;
  }

  //initialize node data and head/tail pointers
  newNode->data = d;
  newNode->np[Head] = NULL;
  newNode->np[Tail] = NULL;

  //if deq is empty, node is head and tail
  if (r->len == 0) {
    r->ht[Head] = newNode;
    r->ht[Tail] = newNode;
  } 
  //if not, insert new node at given end (e) of deq
  else {
    Node previousEndNode = r->ht[e];
    //determine which next pointer of newNode (head/tail) should be updated to point to previousEndNode.
    newNode->np[(e == Head) ? Tail : Head] = previousEndNode;
    //new node becomes the previous end node's next (if at tail) or previous (if at head) neighbor
    previousEndNode->np[e] = newNode; 
    //update head or tail pointer to point to new node
    r->ht[e] = newNode;
  }
  
  //increment deq length
  r->len++;
}


/**
 * Retrieves the data at the i-th index in a doubly linked list represented by `r`, with the end specified by `e`.
 * Deq length unchanged.
 * 
 * @param r: A pointer to the doubly linked list representation (Rep).
 * @param e: Specifies which end to start counting from (Head or Tail).
 * @param i: The index of the element to be retrieved.
 * 
 * @return Data at the i-th index; 0 if an error occurs.
 */
static Data ith(Rep r, End e, int i) {
  //validate index
  if (i < 0 || i >= r->len) {
    ERROR("Invalid index");
    return 0;
  }

  //set starting node (head or tail)
  Node currentNode = r->ht[e];

  //traverse deq to ith index
  for (int j = 0; j < i; j++) {
    //check if currentNode is null
    // if (currentNode == NULL) {
    //   ERROR("Found NULL node while traversing deq");
    //   return 0;
    // }
    //traverse from head or tail based on e given
    currentNode = currentNode->np[(e == Head) ? Tail : Head];
  }

  //ensure node is not null
  if (currentNode == NULL) {
    ERROR("Node is null");
    return 0;
  }
  //return data at ith index
  return currentNode->data;
}


/**
 * Removes a node from either the head or the tail of the deque represented by 'r' 
 * depending on the value given for 'e' (Head or Tail). Returns the data contained in the removed node.
 * Decrements the length of the deq.
 * 
 * @param r  A Rep structure representing the deque.
 * @param e  The end from which the node will be removed (Head or Tail).
 * 
 * @return   Data (d) from the removed node; error if the deque is empty.
 */
static Data get(Rep r, End e) {
  //check if the deque is empty
  if (r->len == 0) {
    ERROR("Deq is empty - cannot get from empty list");
    return 0;
  }

  //save the node that will be removed
  Node currentNode = (e == Head) ? r->ht[Head] : r->ht[Tail];

  //check if the node is null
  if (currentNode == NULL) {
    ERROR("Node is null");
    return 0;
  }

  //save the data from the node to be removed
  Data d = currentNode->data;

  //update head or tail pointer to point to the next or previous node depending on e
  r->ht[e] = currentNode->np[(e == Head) ? Tail : Head];

  //if deq has one node, head and tail should be null
  if (r->ht[e] == NULL) {
    r->ht[(e == Head) ? Tail : Head] = NULL;
  } 
  //otherwise, the backward pointer from the new head/tail should be null
  else {
    r->ht[e]->np[(e == Head) ? Head : Tail] = NULL;
  }

  //free node memory
  free(currentNode);
  //decrement deq length
  r->len--;
  //return data
  return d;
}



/**
 * Removes a node with specified data from a list, and decrement the list length if a match is found.
 *
 * @param r: Pointer to the representation of the list.
 * @param e: Specifies the end of the list to start (Head or Tail).
 * @param d: The data to be removed from the list.
 *
 * @return: Returns the data that was removed or the original data if not found.
 */
static Data rem(Rep r, End e, Data d) {
  
  //check if list is empty
  if (!r || r->len == 0) {
    return 0;
  }

  //initialize the currentNode variable based on the value of the e (end) parameter given
  Node currentNode = (e == Head) ? r->ht[Head] : r->ht[Tail];

  //loop through the list to find the node with the specified data
  while (currentNode) {
    
    //check if current node data matches the data sought
    if (currentNode->data == d) {
      
      //if found, get previous and next nodes for current node
      Node prevNode = currentNode->np[Head];
      Node nextNode = currentNode->np[Tail];
      
      //update the 'next' pointer of the previous node so it points to node after current node
      if (prevNode) {
        prevNode->np[Tail] = nextNode;
      } 
      //if currentNode is head, update head pointer to next node
      else {
        r->ht[Head] = nextNode;
      }

      //set 'previous' pointer of the next node so it points to the node before the current node
      if (nextNode) {
        nextNode->np[Head] = prevNode;
      } 
      //if currentNode is tail, update tail pointer to previous node
      else {
        r->ht[Tail] = prevNode;
      }

      //store data from current node to be removed
      Data removedData = currentNode->data;
      //free current node memory
      free(currentNode);
      //decrement deq after removal of current node
      r->len--;
      //return data removed
      return removedData;
    }

    //move to the next node based on whether search done from head or tail
    currentNode = (e == Head) ? currentNode->np[Tail] : currentNode->np[Head];
  }

  //if node not found
  return 0;
}

/* Function to initialize a new doubly-ended queue */
extern Deq deq_new() {
  Rep r = (Rep)malloc(sizeof(*r));
  if (!r) ERROR("malloc() failed");
  r->ht[Head] = 0;
  r->ht[Tail] = 0;
  r->len = 0;
  return r;
}

/* Function to return the length of the doubly-ended queue */
extern int deq_len(Deq q) { return rep(q)->len; }

/* Function to put data at the head of the doubly-ended queue */
extern void deq_head_put(Deq q, Data d) { put(rep(q), Head, d); }

/* Function to get data from the head of the doubly-ended queue */
extern Data deq_head_get(Deq q)         { return get(rep(q), Head); }

/* Function to get the ith data from the head */
extern Data deq_head_ith(Deq q, int i)  { return ith(rep(q), Head, i); }

/* Function to remove data from the head */
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q), Head, d); }

/* Function to put data at the tail of the doubly-ended queue */
extern void deq_tail_put(Deq q, Data d) { put(rep(q), Tail, d); }

/* Function to get data from the tail of the doubly-ended queue */
extern Data deq_tail_get(Deq q)         { return get(rep(q), Tail); }

/* Function to get the ith data from the tail */
extern Data deq_tail_ith(Deq q, int i)  { return ith(rep(q), Tail, i); }

/* Function to remove data from the tail */
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q), Tail, d); }

/* Function to apply a mapping function on each data element of the doubly-ended queue */
extern void deq_map(Deq q, DeqMapF f) {
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail])
    f(n->data);
}

/* Function to delete the doubly-ended queue */
extern void deq_del(Deq q, DeqMapF f) {
  if (f) deq_map(q, f);
  Node curr = rep(q)->ht[Head];
  while (curr) {
    Node next = curr->np[Tail];
    free(curr);
    curr = next;
  }
  free(q);
}

/* Function to convert the doubly-ended queue to a string representation */
extern Str deq_str(Deq q, DeqStrF f) {
  char *s = strdup("");
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail]) {
    char *d = f ? f(n->data) : n->data;
    char *t; asprintf(&t, "%s%s%s", s, (*s ? " " : ""), d);
    free(s); s = t;
    if (f) free(d);
  }
  return s;
}
