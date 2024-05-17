#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h> 
#include "para-pat.h"

bool stage1_complete = false;   //flag for when the first stage completes its execution
bool stage2_complete = false;   //flag for when the second stage completes its execution

//creates a farm pattern
TaskQueue* create_farm(int n_workers, void* (*worker_func)(void*), TaskQueue* task_queue){
    WorkerTask* worker_task = malloc(sizeof(WorkerTask));
    worker_task->func_ptr.worker_func = worker_func;
    worker_task->input_queue = task_queue;
    worker_task->output_queue = init_queue();
    pthread_t workers[n_workers];

    for(int i = 0; i < n_workers; i++){
        pthread_create(&workers[i], NULL, worker, (void*)worker_task);
    }

    for(int j = 0; j < n_workers; j++){
        pthread_join(workers[j], NULL);
    }

    destroy_queue(worker_task->input_queue);

    return worker_task->output_queue;
}

//creates a pipeline pattern
TaskQueue* create_pipeline(void* (*stage1_func)(void*), void* (*stage2_func)(void*), void* (*stage3_func)(void*), TaskQueue* task_queue){
    WorkerTask* worker_task1 = malloc(sizeof(WorkerTask));
    worker_task1->func_ptr.stage1_func = stage1_func;
    worker_task1->input_queue = task_queue;
    worker_task1->output_queue = init_queue();

    pthread_t stage1_thread;
    pthread_create(&stage1_thread, NULL, stage1, worker_task1);

    WorkerTask* worker_task2 = malloc(sizeof(WorkerTask));
    worker_task2->func_ptr.stage2_func = stage2_func;
    worker_task2->input_queue = worker_task1->output_queue;
    worker_task2->output_queue = init_queue();

    pthread_t stage2_thread;
    pthread_create(&stage2_thread, NULL, stage2, worker_task2);

    WorkerTask* worker_task3 = malloc(sizeof(WorkerTask));
    worker_task3->func_ptr.stage3_func = stage3_func;
    worker_task3->input_queue = worker_task2->output_queue;
    worker_task3->output_queue = init_queue();

    pthread_t stage3_thread;
    pthread_create(&stage3_thread, NULL, stage3, worker_task3);

    pthread_join(stage1_thread, NULL);
    pthread_join(stage2_thread, NULL);
    pthread_join(stage3_thread, NULL);  

    destroy_queue(worker_task1->input_queue);
    destroy_queue(worker_task2->input_queue);
    destroy_queue(worker_task3->input_queue);

    return worker_task3->output_queue;
}

//wrapper function for the worker function
void* worker(void* arg){
    printf("Running worker thread\n");
    WorkerTask* task = (WorkerTask*)arg;
    TaskQueue* input_queue = task->input_queue;
    TaskQueue* output_queue = task->output_queue;
    unsigned long long* param;
    unsigned long long* result;

    while(true){
        if(input_queue->size == 0){
            printf("Queue empty\n");
            break;
        }

        param = get_task(input_queue);
        result = (unsigned long long*)malloc(sizeof(unsigned long long));
        *result = (task->func_ptr.worker_func)(*param);

        put_task(output_queue, result);
    }

    return NULL;
}

//wrapper fuction for the first stage
void* stage1(void* arg) {
    printf("Running worker stage1\n");
    WorkerTask* task = (WorkerTask*)arg;
    TaskQueue* input_queue = task->input_queue;
    TaskQueue* output_queue = task->output_queue;
    int param;
    char* result;

    while(true){
        if(input_queue->size == 0){
            printf("(Stage1) queue empty\n");
            stage1_complete = true;
            break;
        }

        param = *(int*)get_task(input_queue);
        result = (task->func_ptr.stage1_func)(param);
        put_task(output_queue, result);
    }

    return NULL;
}

//wrapper function for the second stage
void* stage2(void* arg) {
    printf("Running worker stage2\n");
    WorkerTask* task = (WorkerTask*)arg;
    TaskQueue* input_queue = task->input_queue;
    TaskQueue* output_queue = task->output_queue;
    char* param;
    task_t* result;

    while(true){
        if(input_queue->size == 0 && stage1_complete){
            printf("(Stage2) queue empty\n");
            stage2_complete = true;
            break;
        }
        
        param = (char*)get_task(input_queue);
        result = malloc(sizeof(task_t));
        *result = (task->func_ptr.stage2_func)(param);
        put_task(output_queue, result);
    }

    return NULL;
}

//wrapper function for the third stage
void* stage3(void* arg) {
    printf("Running worker stage3\n");
    WorkerTask* task = (WorkerTask*)arg;
    TaskQueue* input_queue = task->input_queue;
    TaskQueue* output_queue = task->output_queue;
    task_t* param;
    unsigned short* result;

    while(true){
        if(input_queue->size == 0 && stage2_complete){
            printf("(Stage3) queue empty\n");
            break;
        }

        param = (task_t*)get_task(input_queue);
        result = (task->func_ptr.stage3_func)(*param);
        put_task(output_queue, result);
    }

    return NULL;
}