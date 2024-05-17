#include <stdio.h>
#include <sys/time.h>
#include "para-pat.h"

unsigned long long fib(unsigned long long n) {
  printf("Executing worker function\n");
  printf("arg received: %llu\n", n);
  unsigned long long  i, Fnew, Fold, temp, ans;
  Fnew = 1;  Fold = 0;

  for ( i = 2; i <= n; i++ ) {
    temp = Fnew;
    Fnew = Fnew + Fold;
    Fold = temp;
  }

  ans = Fnew;

  printf("Evaluated answer: %llu\n", ans);
  
  return ans;
}

int payload1(int i) {
  return (fib(900090000)); 
}

int payload2(int j) {
  return (fib(900090000));    
}

double get_current_time() {
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;

  if (gettimeofday(&tval, NULL) == -1) {
    result = -1.0;
  } else if (!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  } else {
    result = (double)(tval.tv_sec - start) + 1.0e-6 * (tval.tv_usec - startu);
  }
    
  return result;
}

int main () {
  int choice;   //choice of parallel pattern to use
  int worker_n = 10;   //number of workers to run in parallel
  int task_n = 10;   //number of tasks to process
  unsigned long long value = 1000;   //task data 
  double beginning, end;

  printf("Choose the pattern to use:\n 0: None\n 1: Farm\n");
  scanf("%i", &choice);

  if (choice == 0) {   //running without the farm pattern
    beginning = get_current_time();

    for (int i = 0; i < task_n; i++) {
      unsigned long long output = fib(value);
      printf("%llu\n", output);
    }

    end = get_current_time();

    printf("Runtime is %f ms\n", (end - beginning) * 1000);

    return 0;
  } else if (choice == 1){   //running with the farm pattern
    TaskQueue* input_queue = init_queue();   //input queue for the farm
    TaskQueue* output_queue;   //output queue for the farm

    for (int i = 0; i < task_n; i++) {
      unsigned long long* param = malloc(sizeof(unsigned long long));
      *param = value;
      put_task(input_queue, param);   //adds task data to the input queue
    }

    beginning = get_current_time();
    
    output_queue = create_farm(worker_n, fib, input_queue);   //runs the farm patterns 

    end = get_current_time();

    TaskData* current_task = output_queue->head;

    printf("Output queue:\n");
    while (current_task != NULL) {
        printf("%llu\n", *(unsigned long long*)(current_task->data));
        current_task = current_task->next;
    }

    printf("Runtime is %f ms\n", (end - beginning) * 1000);

    destroy_queue(output_queue);

    return 0;
  } else {
    printf("Invalid chioce.");
    return 1;
  }
}