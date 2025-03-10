#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t forks_mutex[5];
pthread_cond_t forks_cond[5];

int get_rand(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void pickup_forks(int philosopher_number) {
    // Wait for forks
}

void return_forks(int philosopher_number) {
    // Signal forks
}

void forks_mutex_init() {
    for(int i = 0; i < 5; i++) {
        pthread_mutex_init(&forks_mutex[i], NULL);
    }
}

void forks_mutex_destroy() {
    for(int i = 0; i < 5; i++) {
        pthread_mutex_destroy(&forks_mutex[i]);
    }
}

void forks_cond_init() {
    for(int i = 0; i < 5; i++) {
        pthread_cond_init(&forks_cond[i], NULL);
    }
}

void forks_cond_destroy() {
    for(int i = 0; i < 5; i++) {
        pthread_cond_destroy(&forks_cond[i]);
    }
}

void* philosopher(void* id) {
    printf("Philosopher %d created.\n", *(int*)id);
}

int main() {
    srand(time(NULL));
    forks_mutex_init();
    forks_cond_init();

    // Create philosopher threads passing each an id.
    pthread_t thread_ids[5];
    for(int i = 0; i < 5; i++) {
        int* idPtr = malloc(sizeof(*idPtr));
        *idPtr = i;
        pthread_create(&thread_ids[i], NULL, &philosopher, idPtr);
    }

    // Close all philosopher threads.
    for(int i = 0; i < 5; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    forks_mutex_destroy();
    forks_cond_destroy();
    return 0;
}