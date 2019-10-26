#include "list.h"
#include <stdlib.h>


list_t* initList()
{
    list_t* newList = (list_t*) malloc(sizeof(list_t));
    newList->start = NULL;
    newList->end = NULL;
    newList->size = 0;

    return newList;
}

void addList(list_t* list, void* data)
{
    node_t* newNode = (node_t*) malloc(sizeof(node_t));
    newNode->data = data;
    newNode->next = NULL;
    
    if(list->start == NULL)
    {
        list->start = newNode;
        list->end = newNode;
    }
    else
    {
     list->end->next = newNode;
     list->end = newNode;   
    }
    list->size += 1;
}

node_t* findList(list_t* list, int index)
{
    node_t *node = list->start;

    if (index >= list->size || index < 0) 
    {
        return NULL;
    }

    for (int i = 0; i < index; i++)
    {
        node = node->next;
    }

    return node;
}

void* getList(list_t* list, int index)
{
    node_t* node = findList(list, index);

    if(node == NULL) 
    {
        return NULL;
    }

    return node->data;
}

int removeList(list_t* list, int index)
{
    node_t* previousnode = findList(list, index - 1);
    node_t *currentnode = findList(list, index);

    if (currentnode == NULL)
    {
        return 0;
    }
    if (index == 0)
    {
        list->start = currentnode->next;
    }

    else
    {
        previousnode->next = currentnode->next;
    }

    free(currentnode);
    return 1;
}