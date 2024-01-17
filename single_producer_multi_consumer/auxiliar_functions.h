#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 10000
#define true 1
#define false 0

typedef struct messageT {
  int value;
  int available;
} message;

typedef struct consumerArgumentsT {
  message *matrix;
  int *return_value;
} consumerArguments;

struct timespec timer_start();

long timer_end(struct timespec start_time);

void *consumer_thread(void *data);

void *producer_thread(void *data);
