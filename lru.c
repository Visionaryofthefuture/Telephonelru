#include"lru.h"
#include<stdlib.h>

lrucache * init(int capacity)
{
    lrucache * node = (lrucache *)malloc(sizeof(lrucache));
    if(!node)
    {
        return NULL;
    }
    node->capacity = capacity;
    node->count = 0;
    node->head = node->tail = NULL;
    return node;
}