#include "task_queue.h"
#include "convolution.h"

typedef struct WorkerTask{
    union {
        void* (*worker_func)(void*);   //for fib()
        char* (*stage1_func)(int);   //for read_image()
        task_t (*stage2_func)(char*);   //for read_image_and_mask()
        unsigned short* (*stage3_func)(task_t);   //for process_image()
    } func_ptr;   //function pointer
    TaskQueue* input_queue;   //input queue
    TaskQueue* output_queue;   //output queue
} WorkerTask;

TaskQueue* create_farm(int n_workers, void* (*worker_func)(void*), TaskQueue* task_queue);

TaskQueue* create_pipeline(void* (*stage1_func)(void*), void* (*stage2_func)(void*), void* (*stage3_func)(void*), TaskQueue* task_queue);

void* worker(void* arg);

void* stage1(void* arg);

void* stage2(void* arg);

void* stage3(void* arg);