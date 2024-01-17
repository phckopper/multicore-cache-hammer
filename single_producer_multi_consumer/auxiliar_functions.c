#include "auxiliar_functions.h"

struct timespec timer_start() {
  struct timespec start_time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
  return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time) {
  struct timespec end_time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
  long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 +
                     (end_time.tv_nsec - start_time.tv_nsec);
  return diffInNanos;
}

void *consumer_thread(void *data) {
  consumerArguments *arguments = (consumerArguments *)data;
  message *matrix = arguments->matrix;
  int *return_value = arguments->return_value;
  int accumulator = 0;
  int i = 0;
  while (i < MATRIX_SIZE - 1) {
    if (matrix[i].available) {
      accumulator += matrix[i].value;
      i++;
    }
  }
  if (accumulator == matrix[MATRIX_SIZE - 1].value) {
    *return_value = true;
  } else {
    *return_value = false;
  }

  return NULL;
}

void *producer_thread(void *data) {
  message *matrix = (message *)data;
  int accumulator = 0;
  srand(time(NULL));
  for (int i = 0; i < MATRIX_SIZE - 1; i++) {
    int temp = rand() % 10;
    matrix[i].value = temp;
    matrix[i].available = true;
    accumulator += temp;
  }
  matrix[MATRIX_SIZE - 1].value = accumulator;
  return NULL;
}
