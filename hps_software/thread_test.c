#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int count = 0;

void* func1(void* args) {
    pthread_mutex_lock(&mutex);
    while (count < 100) {
        count++;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* func2(void* args) {
    pthread_mutex_lock(&mutex);
    while (count < 100) {
        printf("%d\n", count);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, &func1, NULL);
    pthread_create(&t2, NULL, &func2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}