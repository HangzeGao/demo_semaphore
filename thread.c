#include "common.h"

void SEM_init(SEM_T *sem, int pshared, unsigned int value)
{
    // 当pshared不为0时，此信号量在进程间共享，否则只在当前进程的所有线程间共享
#ifdef USE_SEM
    sem_init(sem, pshared, value);
#else
    sem->value = value;
    if (pshared == 0) {
        pthread_mutex_init(&sem->mutex, NULL);
        pthread_cond_init(&sem->cond, NULL);
    } else {
        pthread_mutexattr_t mutexAttr;
        pthread_condattr_t condAttr;
        pthread_mutexattr_init(&mutexAttr);
        pthread_condattr_init(&condAttr);
        pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
        pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&sem->mutex, &mutexAttr);
        pthread_cond_init(&sem->cond, &condAttr);
    }
#endif
}

void SEM_wait(SEM_T *sem)
{
#ifdef USE_SEM
    sem_wait(sem);
#else
    pthread_mutex_lock(&sem->mutex);
    sem->value--;
    while(sem->value < 0)
        pthread_cond_wait(&sem->cond, &sem->mutex);
    pthread_mutex_unlock(&sem->mutex);
#endif
}

void SEM_post(SEM_T *sem)
{
#ifdef USE_SEM
    sem_post(sem);
#else
    pthread_mutex_lock(&sem->mutex);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
#endif
}

void SEM_destroy(SEM_T *sem)
{
#ifdef USE_SEM
    sem_destroy(sem);
#else
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    sem->value = 0;
#endif
}
