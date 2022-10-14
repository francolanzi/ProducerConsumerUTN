#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

#define MAX 1000

sem_t empty;
sem_t full;
sem_t mutex;

int produce() {
  sleep(1);
  return rand() % 10;
}

void consume(int value) {
  sleep(1);
  printf("%d ", value);
  fflush(stdout);
}

void *producer(void * arg) {
  buffer_t *buffer = (buffer_t *) arg;
  for (int i = 0; i < MAX; i++) {
    int value = produce();
    sem_wait(&empty);
    sem_wait(&mutex);
    buffer_insert(buffer, value);
    sem_post(&mutex);
    sem_post(&full);
  }
  pthread_exit(NULL);
}

void *consumer(void * arg) {
  buffer_t *buffer = (buffer_t *) arg;
  for (int i = 0; i < MAX; i++) {
    sem_wait(&full);
    sem_wait(&mutex);
    int value = buffer_remove(buffer);
    sem_post(&mutex);
    sem_post(&empty);
    consume(value);
  }
  pthread_exit(NULL);
}

int main() {
  pthread_t pro, con;
  buffer_t buffer;
  srand(time(NULL));
  buffer_init(&buffer);
  sem_init(&empty, 0, BUFF_SIZE);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);
  pthread_create(&pro, NULL, &producer, (void *) &buffer);
  pthread_create(&con, NULL, &consumer, (void *) &buffer);
  pthread_join(pro, NULL);
  pthread_join(con, NULL);
  sem_destroy(&full);
  sem_destroy(&empty);
  sem_destroy(&mutex);
}