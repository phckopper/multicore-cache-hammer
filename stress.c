#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MATRIX_SIZE 10000
#define ITERS 100000

int pingpong = 1;
int matrix_size, iters;

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

void incrementerThread1(int *matrix) {
    for(int i = 0; i < iters; i++) {
        while(pingpong != 1);
        for(int j = 0; j < matrix_size; j++)
            matrix[j]++;
        pingpong = 2;
    }
}

void incrementerThread2(int *matrix) {
    for(int i = 0; i < iters; i++) {
        while(pingpong != 2)
        for(int j = 0; j < matrix_size; j++)
            matrix[j]++;
        pingpong = 1;
    }
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        puts("syntax: <cpu1> <cpu2> <matrix size> <iters>");
        exit(EXIT_FAILURE);
    }

    int cpuPinOne = atoi(argv[1]);
    int cpuPinTwo = atoi(argv[2]);

    matrix_size = atoi(argv[3]);
    iters = atoi(argv[4]);

    cpu_set_t cpusetOne;
    cpu_set_t cpusetTwo;
    
    pthread_t threadOne;
    pthread_t threadTwo;

    int rc;
    int *matrix = malloc(MATRIX_SIZE * sizeof(int));

    rc = pthread_create(&threadOne, NULL, incrementerThread1, (void *) matrix);
    if(rc != 0)
        perror("Creating thread one failed");
    rc = pthread_create(&threadTwo, NULL, incrementerThread2, (void *) matrix);
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
