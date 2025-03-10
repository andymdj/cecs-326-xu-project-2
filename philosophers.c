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
pthread_mutex_t forks_mutex;
pthread_cond_t forks_cond;

int sleep_rand() {
    sleep(rand() % (MAX_WAIT - MIN_WAIT + 1) + MIN_WAIT);
}

void print_forks() {
    char forks_str[NUM_PHILOSOPHERS*3];
    for(int i = 0; i < NUM_PHILOSOPHERS*3; i++) {
        forks_str[i] = 'a';
    }
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int char_index = 1 + i * 3;
        forks_str[char_index] = forks[i] ? i+48 : ' ';
        forks_str[char_index + 1] = forks_str[char_index - 1] = ' ';
    }
    forks_str[0] = '[';
    forks_str[NUM_PHILOSOPHERS*3 - 1] = ']';
    printf("%s\n", forks_str);
}

void forks_init() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        forks[i] = true;
    }
}

void pickup_forks(int ph_id) {
    // Wait for forks
    pthread_mutex_lock(&forks_mutex);
    while(forks[ph_id] == false || forks[(ph_id + 1) % NUM_PHILOSOPHERS] == false) {
        printf("Philosopher %d waiting: ", ph_id);
        print_forks();
        pthread_cond_wait(&forks_cond, &forks_mutex);
    }
    forks[ph_id] = forks[(ph_id + 1) % NUM_PHILOSOPHERS] = false;
    printf("Philosopher %d eating:  ", ph_id);
    print_forks();
    pthread_mutex_unlock(&forks_mutex);
    sleep_rand();
}

void return_forks(int ph_id) {
    // Signal forks
    pthread_mutex_lock(&forks_mutex);
    forks[ph_id] = forks[(ph_id + 1) % NUM_PHILOSOPHERS] = true;
    printf("Philosopher %d done:    ", ph_id);
    print_forks();
    pthread_mutex_unlock(&forks_mutex);
    pthread_cond_signal(&forks_cond);
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
    pthread_mutex_init(&forks_mutex, NULL);
    pthread_cond_init(&forks_cond, NULL);

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

    pthread_mutex_destroy(&forks_mutex);
    pthread_cond_destroy(&forks_cond);
    return 0;
}