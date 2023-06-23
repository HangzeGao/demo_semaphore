#include "common.h"

struct QueueSt {
    SEM_T mutex;       // 互斥锁
    SEM_T empty;       // 信号量（空）
    SEM_T full;        // 信号量（满）
    DATA *base;         // 指向队列空间的基址
    int size;           // 队列总数量
    int front;          // 头指针
    int rear;           // 尾指针
};

// 初始化
QUEUE *QUEUE_Create(int pshared, int size)
{
    QUEUE *queue = NULL;
    queue = (QUEUE *)malloc(sizeof(QUEUE));
    memset(queue, 0, sizeof(QUEUE));
    SEM_init(&(queue->mutex), pshared, 1);
    SEM_init(&(queue->empty), pshared, size);
    SEM_init(&(queue->full), pshared, 0);
    queue->base = (DATA *)calloc(size, sizeof(DATA));
    queue->size = size;
    queue->front = 0;
    queue->rear = 0;
    return queue;
}

// 入队
void QUEUE_Enqueue(QUEUE *queue, DATA data)
{
    // 等待信号量（空） -1
    SEM_wait(&(queue->empty));
    SEM_wait(&(queue->mutex));
    if (queue == NULL) {
        goto END;
    }
    // 判断是否已满
    if ((queue->rear + 1) % queue->size == queue->front) {
        goto END;
    }
    queue->base[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->size;
END:
    SEM_post(&(queue->mutex));
    // 发送信号量（满） + 1
    SEM_post(&(queue->full));
    return;
}

// 出队
void QUEUE_Dequeue(QUEUE *queue, DATA *data)
{
    // 等待信号量（满） -1
    SEM_wait(&(queue->full));
    SEM_wait(&(queue->mutex));
    if (queue == NULL) {
        goto END;
    }
    // 判断是否已空
    if (queue->rear == queue->front) {
        goto END;
    }
    *data = queue->base[queue->front];
    queue->front = (queue->front + 1) % queue->size;
END:
    SEM_post(&(queue->mutex));
    // 发送信号量（空） + 1
    SEM_post(&(queue->empty));
    return;
}

// 销毁
void QUEUE_Destroy(QUEUE *queue)
{
    if (queue == NULL) {
        return;
    }
    SEM_destroy(&(queue->mutex));
    SEM_destroy(&(queue->empty));
    SEM_destroy(&(queue->full));
    free(queue->base);
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    free(queue);
}

int QUEUE_GetSize(QUEUE *queue)
{
    if (queue == NULL) {
        return 0;
    }
    return queue->size;
}

//打印循环队列中的数据
void QUEUE_Show(QUEUE *queue)
{
    SEM_wait(&(queue->mutex));
    printf("---------SHOW\n");
    for(int i = queue->front; i != queue->rear;) {
        printf("%c ", queue->base[i]);
        i = (i + 1) % queue->size;
    }
    printf("\n---------END");
    printf("\n");
    SEM_post(&(queue->mutex));
}
