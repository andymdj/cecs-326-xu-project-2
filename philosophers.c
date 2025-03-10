#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5
#define MIN_WAIT 1
#define MAX_WAIT 3

// A boolean array to act as a shared resource for the philosophers.
// A true indicates the fork is able to be picked up, false if not.
bool forks[NUM_PHILOSOPHERS];

// The mutex and conditional variables for the pickup_forks and return_forks
// funcitons.
pthread_mutex_t forks_mutex;
pthread_cond_t forks_cond;

// Sleeps for a random amount of time between MIN_WAIT and MAX_WAIT seconds.
int sleep_rand() {
    sleep(rand() % (MAX_WAIT - MIN_WAIT + 1) + MIN_WAIT);
}

// Prints the forks array in the format:
//      [0  1  2  3  4]
// If the fork is not able to be picked up, it prints a space, e.g.:
//      [      2      ]
// Here only fork 2 is available to be picked up
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

// Initialize the forks array to all be true values.
void forks_init() {
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        forks[i] = true;
    }
}

// This locks the forks_mutex and checks if the forks of the philosopher's ID
// and the philosopher's ID+1 are taken. If either of them are taken, the 
// thread waits for a signal on forks_cond. When it gets a signal, it will loop
// through the while loop and check if the forks are available again. Once
// they're both available, the while loop will break and the philosopher will
// set both of its forks to be false before unlocking the mutex.
//
// NOTE: if the philosopher's ID is max, it checks for the max and 0th index.
void pickup_forks(int ph_id) {
    pthread_mutex_lock(&forks_mutex);
    while(forks[ph_id] == false || forks[(ph_id + 1) % NUM_PHILOSOPHERS] == false) {
        printf("Philosopher %d waiting:  ", ph_id);
        print_forks();
        pthread_cond_wait(&forks_cond, &forks_mutex);
    }
    // Pick up forks.
    forks[ph_id] = forks[(ph_id + 1) % NUM_PHILOSOPHERS] = false;
    printf("Philosopher %d eating:   ", ph_id);
    print_forks();
    pthread_mutex_unlock(&forks_mutex);
    sleep_rand();
}

// This locks the forks_mutex and sets the forks of the philosopher's ID
// and the philosopher's ID+1 to be true, i.e. the philosopher setting
// their forks down. Once the forks are set down, the philosopher signals
// on forks_cond for any philosophers waiting to check the forks array.
void return_forks(int ph_id) {
    pthread_mutex_lock(&forks_mutex);
    // Put down forks.
    forks[ph_id] = forks[(ph_id + 1) % NUM_PHILOSOPHERS] = true;
    printf("Philosopher %d thinking: ", ph_id);
    print_forks();
    pthread_mutex_unlock(&forks_mutex);
    pthread_cond_signal(&forks_cond);
    sleep_rand();
}

// The function to run when a new thread is created.
// The id is passed from main and is the philosopher's number.
void* philosopher(void* id) {
    printf("Philosopher %d created.\n", *(int*)id);
    while(true) {
        pickup_forks(*(int*)id);
        return_forks(*(int*)id);
    }
}

int main() {
    // Seed the RNG and initialize the forks array, forks mutex, and
    // forks conditional variable.
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

    // Technically the following code will never run but here it is anyway.

    // Close all philosopher threads.
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Destroy the forks mutex and forks conditional variable.
    pthread_mutex_destroy(&forks_mutex);
    pthread_cond_destroy(&forks_cond);
    return 0;
}