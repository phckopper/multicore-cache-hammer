#define _GNU_SOURCE
#include "auxiliar_functions.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    puts("syntax: <cpu0> <cpu1> <cpu2>");
    exit(EXIT_FAILURE);
  }

  int cpuPinZero = atoi(argv[1]);
  int cpuPinOne = atoi(argv[2]);
  int cpuPinTwo = atoi(argv[3]);

  cpu_set_t cpusetOne;
  cpu_set_t cpusetZero;
  cpu_set_t cpusetTwo;

  pthread_t threadZero;
  pthread_t threadOne;
  pthread_t threadTwo;

  int rc;
  message *matrix = malloc(MATRIX_SIZE * sizeof(message));

  for (int i = 0; i < MATRIX_SIZE - 1; i++) {
    matrix[i].available = false;
  }
  matrix[MATRIX_SIZE - 1].available = true;

  CPU_ZERO(&cpusetOne);
  CPU_ZERO(&cpusetZero);
  CPU_ZERO(&cpusetTwo);

  CPU_SET(cpuPinZero, &cpusetZero);
  CPU_SET(cpuPinOne, &cpusetOne);
  CPU_SET(cpuPinTwo, &cpusetTwo);

  int first_return_value = 0;
  int second_return_value = 0;
  consumerArguments first_thread_arguments;
  consumerArguments second_thread_arguments;
  first_thread_arguments.matrix = matrix;
  second_thread_arguments.matrix = matrix;
  first_thread_arguments.return_value = &first_return_value;
  second_thread_arguments.return_value = &second_return_value;

  rc = pthread_create(&threadZero, NULL, producer_thread, (void *)matrix);
  if (rc != 0)
    perror("Creating thread zero failed");
  rc = pthread_create(&threadOne, NULL, consumer_thread,
                      (void *)&first_thread_arguments);
  if (rc != 0)
    perror("Creating thread one failed");
  rc = pthread_create(&threadTwo, NULL, consumer_thread,
                      (void *)&second_thread_arguments);
  if (rc != 0)
    perror("Creating thread two failed");

  rc = pthread_setaffinity_np(threadZero, sizeof(cpusetZero), &cpusetZero);
  if (rc != 0)
    perror("Setting affinity for thread zero failed");
  rc = pthread_setaffinity_np(threadOne, sizeof(cpusetOne), &cpusetOne);
  if (rc != 0)
    perror("Setting affinity for thread one failed");
  rc = pthread_setaffinity_np(threadTwo, sizeof(cpusetOne), &cpusetTwo);
  if (rc != 0)
    perror("Setting affinity for thread two failed");

  struct timespec start_time = timer_start();

  pthread_join(threadZero, NULL);
  pthread_join(threadOne, NULL);
  pthread_join(threadTwo, NULL);

  long time_taken = timer_end(start_time);

  if (first_return_value && second_return_value) {
    printf("Benchmark took %lu ns\n", time_taken);
  } else {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      printf("Item %d: %d\n", j, matrix[j].value);
    }
  }

  free(matrix);

  exit(EXIT_SUCCESS);
}
