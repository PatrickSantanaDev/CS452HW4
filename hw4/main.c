#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "lawn.h"
#include "mole.h"
#include "threads.h"
#include "error.h"

#include "mdeq.h"

// Intializes queue to null
Mdeq queue = NULL;

// <summary>Frees a mole from memory</summary>
// <param = "d">The mole data to delete</param>
static void deleteMole(Data d){
  Mole m = (Mole)d;
  free(m);
}

// <summary>Produces a mole and adds it to the queue</summary>
// <param = "args">The arguments to pass into the produce function (lawn, queue)</param>
static void *produce(void *args)
{
  void **newArgs = args;
  Lawn l = newArgs[0];
  Mdeq queue = newArgs[1];
  Mole mole = mole_new(l, 0, 0);

  mdeq_tail_put(queue, mole);
  return 0;
}

// <summary>Consumes a mole and removes it from the queue</summary>
static void *consume(void *args)
{
  void **newArgs = args;
  Mdeq queue = newArgs[1];
  Mole whackedMole = (Mole) mdeq_head_get(queue);
  mole_whack(whackedMole);
  return 0;
}

int main()
{
  srandom(time(0));

  
  // Number of threads to create and the max number of items in queue
  const int n = 10;
  const int maxCap = 4;

  // Creates a new lawn and queue
  Lawn lawn = lawn_new(0, 0);
  queue = mdeq_new(maxCap);

  // Allocates arguments for the lawn and queue pointers
  void **args = malloc(sizeof(void*) * 2);
  if(!args){
    ERROR("Failed to malloc args");
  }
  args[0] = lawn;
  args[1] = queue;
  
  // Consumes and produces n threads and waits on them to finish
  pthread_t** consumeThreads = create_thread(consume, n, args);
  pthread_t** produceThreads = create_thread(produce, n, args);
  wait_thread(produceThreads, n);
  wait_thread(consumeThreads, n);
 

  lawn_free(lawn);
  free(args);
  mdeq_del(queue, &deleteMole);
}