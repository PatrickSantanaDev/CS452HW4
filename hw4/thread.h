typedef void* (*TFunction)(void*);

pthread_t** create_thread(TFunction, int, void* arg);
void wait_thread(pthread_t** threads, int n);