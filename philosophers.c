#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* philosopher(void* id) {
    printf("Philosopher %d\n", *(int*)id);
}

int main() {
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
    return 0;
}