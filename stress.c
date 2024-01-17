#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MATRIX_SIZE 100000
#define ITERS 10000

pthread_mutex_t mutex;

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

void incrementerThread(int *matrix) {
    for(int i = 0; i < ITERS; i++) {
        pthread_mutex_lock(&mutex);
        for(int j = 0; j < MATRIX_SIZE; j++)
            matrix[j]++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        puts("syntax: <cpu1> <cpu2>");
        exit(EXIT_FAILURE);
    }

    int cpuPinOne = atoi(argv[1]);
    int cpuPinTwo = atoi(argv[2]);

    cpu_set_t cpusetOne;
    cpu_set_t cpusetTwo;
    
    pthread_t threadOne;
    pthread_t threadTwo;

    pthread_mutex_init(&mutex, NULL);

    int rc;
    int *matrix = malloc(MATRIX_SIZE * sizeof(int));

    rc = pthread_create(&threadOne, NULL, incrementerThread, (void *) matrix);
    if(rc != 0)
        perror("Creating thread one failed");
    rc = pthread_create(&threadTwo, NULL, incrementerThread, (void *) matrix);
    if(rc != 0)
        perror("Creating thread two failed");

    CPU_ZERO(&cpusetOne);
    CPU_ZERO(&cpusetTwo);

    CPU_SET(cpuPinOne, &cpusetOne);
    CPU_SET(cpuPinTwo, &cpusetTwo);

    rc = pthread_setaffinity_np(threadOne, sizeof(cpusetOne), &cpusetOne);
    if(rc != 0)
        perror("Setting affinity for thread one failed");
    rc = pthread_setaffinity_np(threadTwo, sizeof(cpusetOne), &cpusetTwo);
    if(rc != 0)
        perror("Setting affinity for thread two failed");

    struct timespec start_time = timer_start();

    pthread_join(threadOne, NULL);
    pthread_join(threadTwo, NULL);

    long time_taken = timer_end(start_time);
    printf("Benchmark took %lu ms\n", time_taken/(1000*1000));

    free(matrix);

    exit(EXIT_SUCCESS);
}
