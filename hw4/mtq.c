#include "mtq.h"
#include "pthread.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

// Defines the condition to signal to wake up a thread
#define signal(conditionVar)\
    pthread_cond_signal(conditionVar);\

// Thread sleeps until woken up and condition is false
#define waitPattern(condition, cv, mutex);\
while(condition)\
{\
    pthread_cond_wait(cv, mutex);\
}\

// Multithreaded pattern for void patterns in the mdeq
#define voidPattern(rep, condition, function) \
rep = (Mrep) rep;\
pthread_mutex_lock(&rep->mutex);\
waitPattern(condition, &rep->consumed, &rep->mutex);\
function;\
signal(&rep->produced);\
pthread_mutex_unlock(&rep->mutex);\

// Multithreaded pattern for non void returns in the mdeq
#define returnPattern(rep, condition, function, signalConsumed)\
rep = (Mrep) rep;\
pthread_mutex_lock(&rep->mutex);\
waitPattern(condition, &rep->produced, &rep->mutex)\
Data returnVal = (Data) function;\
if(signalConsumed){\
signal(&rep->consumed);\
}\
pthread_mutex_unlock(&rep->mutex);\
return returnVal;\

// Structure to represent the multithreaded queue
typedef struct{
    int maxCapacity;
    pthread_mutex_t mutex;
    pthread_cond_t consumed;
    pthread_cond_t produced;
    Deq q;
} *Mrep;

// <summary>Creates a new multithreaded queue</summary>
// <param = "capacity">The max number of items that can be in the queue at once</param>
// <returns>A new multithreaded deq</returns>
Mdeq mdeq_new(int capacity){
    Mrep mdeq = (Mrep) malloc(sizeof(*mdeq));
    if(!mdeq){
        ERROR("Failed to malloc mdeq");
    }
    mdeq->q = deq_new();
    mdeq->maxCapacity = capacity;
    pthread_mutex_init(&mdeq->mutex, 0);

    if(!&mdeq->mutex){
        ERROR("Failed to initalize the mutex");
    }
    pthread_cond_init(&mdeq->consumed, 0);

    if(!&mdeq->consumed){
        ERROR("Failed to intialize consumed condition variable");
    }
    pthread_cond_init(&mdeq->produced, 0 );

    if(!&mdeq->produced){
        ERROR("Failed to intialize the produced condition variable");
    }

    return (Mdeq) mdeq;
}

// <summary>Wrapped deq put tail function that implements mutually exclusive access</summary>
// <param = "mdeq">The mdeq to add to</param>
// <param = "data">The data to put into the queue</param>
void mdeq_tail_put(Mdeq mdeq, Data d){
    Mrep rep = (Mrep)(mdeq);
    voidPattern(rep, deq_len(rep->q)>= rep->maxCapacity && rep->maxCapacity > 0, deq_tail_put(rep->q, d));

}

// <summary>Wrapped deq head tail function that implements mutually exclusive access</summary>
// <param = "mdeq">The mdeq to add to</param>
// <param = "data">The data to put into the queue</param>
void mdeq_head_put(Mdeq mdeq, Data d){
    Mrep rep = (Mrep)(mdeq);
    voidPattern(rep, deq_len(rep->q)>= rep->maxCapacity && rep->maxCapacity > 0, deq_head_put(rep->q, d));
}

// <summary>Wrapped deq head get function that implements mutually exclusive access</summary>
// <param = "mdeq">The mdeq to retrieve the data from</param>
// <returns>Returns the data that was at the head</returns>
Data mdeq_head_get(Mdeq mdeq){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)==0, deq_head_get(rep->q), 1);
    return returnData;
}

// <summary>Wrapped deq head get function that implements mutually exclusive access</summary>
// <param = "mdeq">The mdeq to retrieve the data from</param>
// <returns>Returns the data that was at the head</returns>
Data mdeq_tail_get(Mdeq mdeq){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)==0, deq_tail_get(rep->q), 1);
    return returnData;
}

// <summary>Wrapped deq head ith function for mutually exclusive access</summary>
// <param = "mdeq">The mdeq to retrieve the data from</param>
// <param = "i">The index to find</param>
// <returns>Returns the data that was at i</returns>
Data mdeq_head_ith(Mdeq mdeq, int i){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)-1<i, deq_head_ith(rep->q, i), 0);
    return returnData;
}

// <summary>Wrapped deq head ith function for mutually exclusive access</summary>
// <param = "mdeq">The mdeq to retrieve the data from</param>
// <param = "i">The index to find</param>
// <returns>Returns the data that was at i</returns>
Data mdeq_tail_ith(Mdeq mdeq, int i){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)-1<i, deq_tail_ith(rep->q, i), 0);
    return returnData;
}

// <summary>Wrapped deq head rem function for mutually exclusive access</summary>
// <param = "mdeq">The mdeq to remove the data from</param>
// <param = "d">The data to find and remove</param>
Data mdeq_head_rem(Mdeq mdeq, Data d){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)==0, deq_head_rem(rep->q, d), 1);
    return returnData;
}
// <summary>Wrapped deq tail rem function for mutually exclusive access</summary>
// <param = "mdeq">The mdeq to remove the data from</param>
// <param = "d">The data to find and remove</param>
Data mdeq_tail_rem(Mdeq mdeq, Data d){
    Mrep rep = (Mrep)(mdeq);
    Data returnData = returnPattern(rep, deq_len(rep->q)==0, deq_tail_rem(rep->q, d), 1);
    return returnData;
}

// <summary>Deconstructs the mdeq</summary>
// <param = "mdeq">The mdeq to deconstruct</param>
// <param = "data">The function to deconstruct any data in the structure</param>
void mdeq_del(Mdeq mdeq, DeqMapF remFunction){
    Mrep rep = (Mrep)(mdeq);
    pthread_mutex_destroy(&rep->mutex);
    pthread_cond_destroy(&rep->produced);
    pthread_cond_destroy(&rep->consumed);
    deq_del(rep->q, remFunction);
    free(rep);
}