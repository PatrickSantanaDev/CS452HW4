#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread.h"
#include "mole.h"
#include "lawn.h"
#include "error.h"

// <summary>Creates n threads running the passed in function</summary>
// <param = "passFunc">The function to run in the newly created thread</param>
// <param = "n">The number of threads to create</param>
// <param = "arg">The arguments to pass to the function</param>
// <returns>A list of thread id's that were created</returns>
pthread_t** create_thread(TFunction passFunc, int n, void *arg){
    pthread_t** threadList = malloc(sizeof(pthread_t*) * (n));
    *threadList = malloc(sizeof(pthread_t)*(n));

    if(!threadList){
        ERROR("Malloc for threadlist failed");
    }

    for(int i =0; i < n; i++){
        pthread_t thread_id = 0;
        int returnVal = pthread_create(&thread_id, NULL, passFunc, arg);

        if(returnVal){
            ERROR("A thread failed to be created");
        }

        *(*(threadList)+i) = thread_id;
    }
    return threadList;
}

// <summary>Waits on n threads to exit</summary>
// <param = "threads">The list of threads to wait on</param>
// <param = "n">The number of threads to wait on</param>
void wait_thread(pthread_t** threads, int n){
    for(int i = 0; i < n; i++){
       pthread_t waitID =  (*threads)[i]; 
       void* exitCode;
       int returnVal = pthread_join(waitID, &exitCode);
       if(returnVal){
           ERROR("An error occured waiting for a thread to exit");
       }
       if(exitCode){
           ERROR("A thread executed abnormally");
       }
    }
    free(*threads);
    free(threads);
}