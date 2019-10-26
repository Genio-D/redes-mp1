#include "queue.h"
#include <stdlib.h>


queue_t* initQueue()
{
    queue_t* queue = (queue_t*) initList();
    return queue;
}

void pushQueue(queue_t* queue, void* data)
{
    addList((list_t*)queue, data);
}

void* popQueue(queue_t* queue)
{
    list_t* list = (list_t*) queue;

    int data = getList(list, 0);
    removeList(list, 0);
    return data;
}