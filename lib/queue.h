#ifndef QUEUE
#define QUEUE

#include "list.h"

typedef list_t queue_t;

queue_t* initQueue();
void pushQueue(queue_t *queue, void* data);
void* popQueue(queue_t *queue);

#endif