#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

typedef void *(*TFunction)(void *);

pthread_t *create_individual_thread(TFunction f, void *arg);
pthread_t **create_threads(TFunction f, int n, void *arg);
void wait_individual_thread(pthread_t *thread);
void wait_threads(pthread_t **threads, int n);

#endif 
