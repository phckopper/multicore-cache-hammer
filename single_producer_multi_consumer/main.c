#define _GNU_SOURCE
#include "auxiliar_functions.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 10
#define true 1
#define false 0

typedef struct consumerArgumentsT {
  int *matrix;
  int *return_value;
} consumerArguments;

void *consumer_thread(void *data) {
  consumerArguments *arguments = (consumerArguments *)data;
  int *matrix = arguments->matrix;
  int *return_value = arguments->return_value;
  int accumulator = 0;
  for (int i = 0; i < MATRIX_SIZE - 1; i++) {
    accumulator += matrix[i];
  }
  if (accumulator == matrix[MATRIX_SIZE - 1]) {
    *return_value = true;
  } else {
    *return_value = false;
  }

  return NULL;
}

void *populate_matrix(void *data) {
  int *matrix = (int *)data;
  int accumulator = 0;
  srand(time(NULL));
  for (int i = 0; i < MATRIX_SIZE - 1; i++) {
    int temp = rand() % 10;
    matrix[i] = temp;
    accumulator += temp;
  }
  matrix[MATRIX_SIZE - 1] = accumulator;
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    puts("syntax: <cpu1> <cpu2>");
    exit(EXIT_FAILURE);
  }

  int cpuPinOne = atoi(argv[1]);
  int cpuPinTwo = atoi(argv[2]);

  cpu_set_t cpusetOne;
  cpu_set_t cpusetTwo;

  pthread_t threadZero;
  pthread_t threadOne;
  pthread_t threadTwo;

  int rc;
  int *matrix = malloc(MATRIX_SIZE * sizeof(int));

  CPU_ZERO(&cpusetOne);
  CPU_ZERO(&cpusetTwo);

  CPU_SET(cpuPinOne, &cpusetOne);
  CPU_SET(cpuPinTwo, &cpusetTwo);

  rc = pthread_create(&threadZero, NULL, populate_matrix, (void *)matrix);
  if (rc != 0)
    perror("Creating thread one failed");

  pthread_join(threadZero, NULL);

  int first_return_value = 0;
  int second_return_value = 0;
  consumerArguments first_thread_arguments;
  consumerArguments second_thread_arguments;
  first_thread_arguments.matrix = matrix;
  second_thread_arguments.matrix = matrix;
  first_thread_arguments.return_value = &first_return_value;
  second_thread_arguments.return_value = &second_return_value;

  rc = pthread_create(&threadOne, NULL, consumer_thread,
                      (void *)&first_thread_arguments);
  if (rc != 0)
    perror("Creating thread one failed");
  rc = pthread_create(&threadTwo, NULL, consumer_thread,
                      (void *)&second_thread_arguments);
  if (rc != 0)
    perror("Creating thread two failed");

  rc = pthread_setaffinity_np(threadOne, sizeof(cpusetOne), &cpusetOne);
  if (rc != 0)
    perror("Setting affinity for thread one failed");
  rc = pthread_setaffinity_np(threadTwo, sizeof(cpusetOne), &cpusetTwo);
  if (rc != 0)
    perror("Setting affinity for thread two failed");

  struct timespec start_time = timer_start();

  pthread_join(threadOne, NULL);
  pthread_join(threadTwo, NULL);

  long time_taken = timer_end(start_time);

  if (first_return_value && second_return_value) {
    printf("Benchmark took %lu ns\n", time_taken);
  } else {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      printf("Item %d: %d\n", j, matrix[j]);
    }
  }

  free(matrix);

  exit(EXIT_SUCCESS);
}
