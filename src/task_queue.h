#include <pthread.h>
#include <stdbool.h>

typedef struct TaskData{   
    void* data;   //function parameter
    struct TaskData* next;   //pointer for the next function parameter in the queue
} TaskData;

typedef struct TaskQueue {
    int size;   //size of the queue
    TaskData* head;   //pointer for the front of the queue
    TaskData* tail;   //pointer for the back of the queue 
    pthread_mutex_t lock;   //lock
    pthread_cond_t cond;   //condition variable
} TaskQueue;

TaskQueue* init_queue();

void put_task(TaskQueue* task_queue, void* task);

void* get_task(TaskQueue* task_queue);

void destroy_queue(TaskQueue* task_queue);