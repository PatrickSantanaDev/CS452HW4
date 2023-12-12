#include "threads.h"
#include "error.h"


/**
 * Creates a single thread that executes the given produce/consume function.
 *
 * @param f pointer to the function the thread will execute.
 * @param arg pointer to the args passed to the function.
 * @return pointer to the created pthread_t struct.
 */
pthread_t *create_individual_thread(TFunction f, void *arg)
{
    // allocate memory for thread
    pthread_t *thread = malloc(sizeof(pthread_t));
    if (!thread)
    {
        ERROR("Memory allocation failed for thread");
    }

    // create thread that will execute f
    int returnVal = pthread_create(thread, NULL, f, arg);
    if (returnVal)
    {
        ERROR("Thread creation failed");
    }

    // return pointer to created thread
    return thread;
}

/**
 * Creates multiple threads that execute the given produce/consume functions.
 *
 * @param f pointer to the function the threads will execute.
 * @param n number of threads created.
 * @param arg pointer to the args passed to the function.
 * @return pointer to array of pointers to created pthread_t structs.
 */
pthread_t **create_threads(TFunction f, int n, void *arg)
{
    // allocate memory for thread pointers
    pthread_t **threads = malloc(sizeof(pthread_t *) * n);
    if (!threads)
    {
        ERROR("Memory allocation failed for thread list");
    }

    // create n threads
    for (int i = 0; i < n; i++)
    {
        threads[i] = create_individual_thread(f, arg);
    }

    // return pointer to first thread in list
    return threads;
}

/**
 * Waits for a thread to terminate and then frees its memory.
 *
 * @param thread pointer to the pthread_t struct of the thread waited for
 */
void wait_individual_thread(pthread_t *thread)
{
    void *exitCode;
    // wait for thread to terminate
    int returnVal = pthread_join(*thread, &exitCode);

    if (returnVal)
    {
        ERROR("An error occurred while waiting for a thread to exit");
    }

    // free mem for the thread
    free(thread);
}

/**
 * Waits for multiple threads to terminate and then frees their memory.
 *
 * @param threads pointer to an array of pointers to the pthread_t structs of the threads waited for.
 * @param n The number of threads to wait for.
 */
void wait_threads(pthread_t **threads, int n)
{
    // wait for each thread to terminate and free the mem
    for (int i = 0; i < n; i++)
    {
        wait_individual_thread(threads[i]);
    }
    // free mem allocated for thread pointers
    free(threads);
}