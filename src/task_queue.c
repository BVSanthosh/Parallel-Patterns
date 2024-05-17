#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "task_queue.h"

//initiates the queue
TaskQueue* init_queue(){
    TaskQueue* task_queue = malloc(sizeof(TaskQueue));

    task_queue->size = 0;
    task_queue->head = NULL;
    task_queue->tail = NULL;
    pthread_mutex_init(&(task_queue->lock), NULL);
    pthread_cond_init(&(task_queue->cond), NULL);

    return task_queue;
}

//pushes a task data to the queue
void put_task(TaskQueue* task_queue, void* data){
    TaskData* new_task = malloc(sizeof(TaskData));
    new_task->data = data;
    new_task->next = NULL;

    pthread_mutex_lock(&(task_queue->lock));

    if(task_queue->size == 0) {
        task_queue->head = new_task;
        task_queue->tail = new_task;
    } else {
        task_queue->tail->next = new_task;
        task_queue->tail = new_task;
    }

    task_queue->size++;
    
    pthread_cond_signal(&(task_queue->cond));
    pthread_mutex_unlock(&(task_queue->lock));
}

//pops a task data from the list
void* get_task(TaskQueue* task_queue){
    pthread_mutex_lock(&(task_queue->lock));

    while (task_queue->size == 0) {
        pthread_cond_wait(&(task_queue->cond), &(task_queue->lock));
    }

    TaskData* task = task_queue->head;
    void* data = task->data;
    task_queue->head = task_queue->head->next;
    task_queue->size--;

    free(task);
    pthread_mutex_unlock(&(task_queue->lock));
    
    return data;
}

//frees the allocated memory
void destroy_queue(TaskQueue* task_queue){
    pthread_mutex_lock(&(task_queue->lock));

    TaskData* current_task = task_queue->head;

    while (current_task != NULL) {
        TaskData* temp = current_task;
        current_task = current_task->next;
        free(temp);
    }

    pthread_mutex_unlock(&(task_queue->lock));
    pthread_mutex_destroy(&(task_queue->lock));
    pthread_cond_destroy(&(task_queue->cond)); 
    free(task_queue);
}