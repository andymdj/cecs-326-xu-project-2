#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5
#define MIN_WAIT 1
#define MAX_WAIT 3

bool forks[NUM_PHILOSOPHERS];
pthread_mutex_t forks_mutex[NUM_PHILOSOPHERS];
pthread_cond_t forks_cond[NUM_PHILOSOPHERS];

int sleep_rand() {
    sleep(rand() % (MAX_WAIT - MIN_WAIT + 1) + MIN_WAIT);
}

void forks_init() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        forks[i] = true;
    }
}

void forks_mutex_init() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks_mutex[i], NULL);
    }
}

void forks_mutex_destroy() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks_mutex[i]);
    }
}

void forks_cond_init() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cond_init(&forks_cond[i], NULL);
    }
}

void forks_cond_destroy() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cond_destroy(&forks_cond[i]);
    }
}

void pickup_forks(int philosopher_number) {
    // Wait for forks
    pthread_mutex_lock(&forks_mutex[philosopher_number]);
    pthread_mutex_lock(&forks_mutex[(philosopher_number + 1) % NUM_PHILOSOPHERS]);
    printf("Philosopher %d eating: [%d, %d]\n", philosopher_number, philosopher_number, (philosopher_number+1)%NUM_PHILOSOPHERS);
    sleep_rand();
}

void return_forks(int philosopher_number) {
    // Signal forks
    printf("Philosopher %d done:   [%d, %d]\n", philosopher_number, philosopher_number, (philosopher_number+1)%NUM_PHILOSOPHERS);
    pthread_mutex_unlock(&forks_mutex[philosopher_number]);
    pthread_mutex_unlock(&forks_mutex[(philosopher_number + 1) % NUM_PHILOSOPHERS]);
    sleep_rand();
}

void* philosopher(void* id) {
    printf("Philosopher %d created.\n", *(int*)id);
    while(true) {
        pickup_forks(*(int*)id);
        return_forks(*(int*)id);
    }
}

int main() {
    srand(time(NULL));
    forks_init();
    forks_mutex_init();
    forks_cond_init();

    // Create philosopher threads passing each an id.
    pthread_t thread_ids[NUM_PHILOSOPHERS];
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int* idPtr = malloc(sizeof(*idPtr));
        *idPtr = i;
        pthread_create(&thread_ids[i], NULL, &philosopher, idPtr);
    }

    // Close all philosopher threads.
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    forks_mutex_destroy();
    forks_cond_destroy();
    return 0;
}