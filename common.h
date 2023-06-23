#ifndef _COMMON_H
#define _COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifdef USE_SEM
# include <semaphore.h>
#endif
#include <unistd.h>

#ifdef USE_SEM
typedef sem_t SEM_T;
#else
typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SEM_T;
#endif

void SEM_init(SEM_T *sem, int pshared, unsigned int value);
void SEM_wait(SEM_T *sem);
void SEM_post(SEM_T *sem);
void SEM_destroy(SEM_T *sem);

typedef int DATA;
typedef struct QueueSt QUEUE;

QUEUE *QUEUE_Create(int pshared, int size);
void QUEUE_Enqueue(QUEUE *queue, DATA data);
void QUEUE_Dequeue(QUEUE *queue, DATA *data);
void QUEUE_Destroy(QUEUE *queue);
int QUEUE_GetSize(QUEUE *queue);
void QUEUE_Show(QUEUE *queue);
#endif