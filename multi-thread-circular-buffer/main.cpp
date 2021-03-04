#include <iostream>
#include <pthread.h>
#include <unistd.h>
#define SIZE 8
#define p 100
using namespace std;

void *producer(void *arg);
void *customer(void *arg);

int buf[SIZE];
int count=0, in= -1, out = -1;

pthread_mutex_t mutex;
pthread_cond_t buffer_has_space, buffer_has_data;

int main(void)
{
    int i;
    pthread_t threads[2];
    pthread_create(&threads[0], NULL, producer, NULL);
    pthread_create(&threads[1], NULL, customer, NULL);

    for(i=0; i<2; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_cond_destroy(&buffer_has_space);
    pthread_cond_destroy(&buffer_has_data);

    pthread_exit(NULL);
}

void *producer(void *arg) {
    int i;
    for(i=1; i<=SIZE; i++) {
        printf("producer data : %d\n", i);
        pthread_mutex_lock(&mutex);
        if(count == SIZE) {
            pthread_cond_wait(&buffer_has_space, &mutex);
        }

        in++;
        in %= SIZE;
        buf[in] = i;
        count++;

        pthread_cond_signal(&buffer_has_data);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void *customer(void *arg) {
    int i, data;

    for(i=1; i<=SIZE; i++) {
        pthread_mutex_lock(&mutex);
        if(count==0) {
            pthread_cond_wait(&buffer_has_data, &mutex);
        }
        out++;
        out %= SIZE;
        data = buf[out];
        count--;

        pthread_cond_signal(&buffer_has_space);
        pthread_mutex_unlock(&mutex);
        printf("        consumer data=%d\n", data);
    }
}
