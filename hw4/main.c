#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "lawn.h"
#include "mole.h"
#include "error.h"

#include <pthread.h>
#include "mtq.h"
#include "threads.h"

// thread function sig
typedef void *(*TFunction)(void *);

// intialize mtq to null
Mtq mtq = NULL;

/**
 * Creates a new mole and enqueues it to a mtq.
 *
 * @param a A pointer to a void pointer array.
 *             a[0] is expected to be a pointer to a mtq object and a[1] is expected to be a pointer to a lawn object.
 *
 * @return null upon success
 */
static void *produce(void *a)
{
    void **arg = a;
    Mtq mtq = arg[0];
    Lawn l = arg[1];
    // add a new mole to the tail of mtq
    mtq_tail_put(mtq, mole_new(l, 0, 0));
    return 0;
}

/**
 * Consumes a mole by removing it from the mtq head and performing a whack.
 *
 * @param a A pointer to a void pointer array.
 *             a[0] is expected to be a pointer to a mtq
 *
 * @return NULL upon success
 */
static void *consume(void *a)
{    
    void **arg = a;
    Mtq mtq = arg[0];
    // retrieve and remove a mole from the head of mtq
    Mole whacked = (Mole)mtq_head_get(mtq);
    // whack mole
    mole_whack(whacked);
    // success
    return 0;
}

/**
 * Frees the memory allocated to a mole object after it has been processed.
 *
 * @param d data object for mole to be deleted.
 */
static void free_mole(Data d)
{
    Mole m = (Mole)d;
    free(m);
}


int main()
{
    srandom(time(0));

    // max capacity of mtq - capacity of four to cause produce congestion
    const int mtqMax = 4;

    // num threads created
    const int n = 15;

    // create new mtq and lawn
    mtq = mtq_new(mtqMax);
    Lawn lawn = lawn_new(0, 0);

    // allocate args for mtq and lawn pointers
    void **threadArgs = malloc(sizeof(void *) * 2);
    if (!threadArgs)
    {
        ERROR("Failed malloc for threadArgs");
    }

    threadArgs[0] = mtq;
    threadArgs[1] = lawn;

    // consume/produce n threads
    pthread_t **produceThreads = create_threads(produce, n, threadArgs);
    pthread_t **consumeThreads = create_threads(consume, n, threadArgs);

    // wait for all threads to finish
    wait_threads(produceThreads, n);
    wait_threads(consumeThreads, n);

    // cleanup
    lawn_free(lawn);
    free(threadArgs);
    mtq_del(mtq, &free_mole);
}
