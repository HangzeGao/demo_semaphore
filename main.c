#include "common.h"

#define pshared 0
#define bsize 26
#define item_count bsize * 4

QUEUE *queue_lower = NULL;
QUEUE *queue_upper = NULL;

void init(void)
{
    queue_lower = QUEUE_Create(pshared, bsize);
    queue_upper = QUEUE_Create(pshared, bsize);
}

void destroy(void)
{
    QUEUE_Destroy(queue_lower);
    QUEUE_Destroy(queue_upper);
}

void *produce(void *args)
{
    DATA data;
    for(int i = 0; i < item_count; i++) {
        data = 'a' + i % bsize;
        QUEUE_Enqueue(queue_lower, data);
    }
    return NULL;
}

void *consume(void *args)
{
    DATA data;
    for(int i = 0; i < item_count; i++) {
        QUEUE_Dequeue(queue_upper, &data);
    }
    return NULL;
}

void *compute(void *args)
{
    int data;
    for(int i = 0; i < item_count; i++) {
        QUEUE_Dequeue(queue_lower, &data);
        data -= ('a' - 'A');
        QUEUE_Enqueue(queue_upper, data);
        QUEUE_Show(queue_lower);
        QUEUE_Show(queue_upper);
    }
    return NULL;
}

int main()
{
    pthread_t pid[3];
    printf("---------START\n");
    init();
    pthread_create(&pid[0], NULL, produce, NULL);
    pthread_create(&pid[1], NULL, compute, NULL);
    pthread_create(&pid[2],NULL, consume, NULL);
    for(int i = 0; i < 3; i++)
        pthread_join(pid[i], NULL);
    printf("---------END\n");
    destroy();
    return 0;
}

// gcc -o main main.c queue.c thread.c -lpthread -W -g -D USE_SEM
