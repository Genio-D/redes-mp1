#include <stdio.h>
#include "list.h"

int main() {
    list_t *list = initList();
    int x = 123;
    addList(list, (void *) &x);
    
    int* y = (int *) getList(list, 0);
    printf("%d", *y);

    return 0;
}