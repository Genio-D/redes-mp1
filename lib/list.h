#ifndef LIST
#define LIST

typedef struct node
{
    void* data;
    struct node* next;
}node_t;

typedef struct
{
    node_t* start;
    node_t* end;
    int size;
}list_t;

list_t *initList();
void addList(list_t *list, void* data);
void* getList(list_t *list, int index);
int removeList(list_t *list, int index);

#endif