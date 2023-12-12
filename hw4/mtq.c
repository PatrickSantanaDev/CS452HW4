#include "error.h"
#include <stdio.h>
#include <stdlib.h>

#include "mtq.h"
#include "pthread.h"

// Structure to represent mtq
typedef struct
{
    int max;                 // max number of items that can be in the mtq at once
    pthread_mutex_t lock;    // ensures mtq is accessed by only one thread at a time/ prevent race conditions
    pthread_cond_t consumed; // signals when data has been consumed from mtq
    pthread_cond_t produced; // signals when new data has been produced to the queue
    Deq q;
} *Mrep;

/**
 * Creates a new mtq with a maximum size.
 *
 * @param mtqMax The maximum number of elements the mtq can hold.
 * @return new mtq object.
 */
Mtq mtq_new(int mtqMax)
{
    Mrep mtq = (Mrep)malloc(sizeof(*mtq));
    if (!mtq)
    {
        ERROR("Failed malloc for mtq");
    }

    mtq->q = deq_new();
    mtq->max = mtqMax;

    if (pthread_mutex_init(&mtq->lock, NULL) != 0)
    {
        ERROR("Failed lock initialization");
    }

    if (pthread_cond_init(&mtq->consumed, NULL) != 0)
    {
        ERROR("Failed initialization of consumed variable");
    }

    if (pthread_cond_init(&mtq->produced, NULL) != 0)
    {
        ERROR("Failed initialization of produced variable");
    }

    return (Mtq)mtq;
}

/**
 * Inserts data at the head of the mtq.
 * This function is thread-safe, meaning it locks the queue during insertion.
 * If the queue is full, it will wait until space is available.
 *
 * @param mtq The mtq where the data will be inserted.
 * @param d The data to insert at the head of the mtq.
 */
void mtq_head_put(Mtq mtq, Data d)
{
    Mrep rep = (Mrep)(mtq);
    pthread_mutex_lock(&rep->lock);

    while (deq_len(rep->q) >= rep->max && rep->max > 0)
    {
        pthread_cond_wait(&rep->consumed, &rep->lock);
    }

    deq_head_put(rep->q, d);
    pthread_cond_signal(&rep->produced);
    pthread_mutex_unlock(&rep->lock);
}

/**
 * Inserts data at the tail of the mtq.
 * This function is thread-safe, meaning it locks the queue during insertion.
 * If the queue is full, it will wait until space is available.
 *
 * @param mtq The mtq where the data will be inserted.
 * @param d The data to insert at the tail of the mtq.
 */
void mtq_tail_put(Mtq mtq, Data d)
{
    Mrep rep = (Mrep)(mtq);
    pthread_mutex_lock(&rep->lock);

    while (deq_len(rep->q) >= rep->max && rep->max > 0)
    {
        pthread_cond_wait(&rep->consumed, &rep->lock);
    }

    deq_tail_put(rep->q, d);
    pthread_cond_signal(&rep->produced);
    pthread_mutex_unlock(&rep->lock);
}

/**
 * Retrieves and removes data from the head of the mtq.
 * This function is thread-safe, meaning it locks the queue during removal.
 * If the queue is empty, it will wait until data is available.
 *
 * @param mtq The mtq to retrieve the data from.
 * @return The data removed from the head of the mtq.
 */
Data mtq_head_get(Mtq mtq)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) == 0)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_head_get(rep->q);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Retrieves and removes data from the tail of the mtq.
 * This function is thread-safe, locking the queue during removal.
 * If the queue is empty, it waits until data is available.
 *
 * @param mtq The mtq to retrieve the data from.
 * @return The data removed from the tail of the mtq.
 */
Data mtq_tail_get(Mtq mtq)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) == 0)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_tail_get(rep->q);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Retrieves an element from a specific position from the head of the mtq.
 * This function is thread-safe, locking the queue during the retrieval.
 * If the specified index is not available, it waits until it is.
 *
 * @param mtq The mtq to retrieve from.
 * @param i The index position from the head of the mtq.
 * @return The data at the ith position from the head of the mtq.
 */
Data mtq_head_ith(Mtq mtq, int i)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) - 1 < i)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_head_ith(rep->q, i);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Retrieves an element from a specific position from the tail of the mtq.
 * This function is thread-safe, locking the queue during the operation.
 * If the specified index is not available, it waits until it is.
 *
 * @param mtq The mtq to retrieve from.
 * @param i The index position from the tail of the mtq.
 * @return The data at the ith position from the tail of the mtq.
 */
Data mtq_tail_ith(Mtq mtq, int i)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) - 1 < i)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_tail_ith(rep->q, i);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Removes an element from the head of the mtq.
 * This function is thread-safe, locking the queue during removal.
 * If the queue is empty, it waits until data is available.
 *
 * @param mtq The mtq to remove from.
 * @param d The data to be found and removed from the head of the mtq.
 * @return The removed data from the head of the mtq.
 */
Data mtq_head_rem(Mtq mtq, Data d)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) == 0)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_head_rem(rep->q, d);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Removes an element from the tail of the mtq.
 * This function is thread-safe, locking the queue during removal.
 * If the queue is empty, it waits until data is available.
 *
 * @param mtq The mtq to remove from.
 * @param d The data to be found and removed from the tail of the mtq.
 * @return The removed data from the tail of the mtq.
 */
Data mtq_tail_rem(Mtq mtq, Data d)
{
    Mrep rep = (Mrep)(mtq);
    Data returnData;

    pthread_mutex_lock(&rep->lock);
    while (deq_len(rep->q) == 0)
    {
        pthread_cond_wait(&rep->produced, &rep->lock);
    }

    returnData = deq_tail_rem(rep->q, d);
    pthread_cond_signal(&rep->consumed);
    pthread_mutex_unlock(&rep->lock);

    return returnData;
}

/**
 * Deallocates memory used by the mtq and destroys mutexes and condition variables.
 * Deq deletion included.
 *
 * @param mtq mtq to be deleted.
 * @param f function pointer for remove elements from the underlying deq.
 */
void mtq_del(Mtq mtq, DeqMapF f)
{
    Mrep rep = (Mrep)(mtq);
    pthread_mutex_destroy(&rep->lock);
    pthread_cond_destroy(&rep->produced);
    pthread_cond_destroy(&rep->consumed);
    deq_del(rep->q, f);
    free(rep);
}