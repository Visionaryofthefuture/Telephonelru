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

void updateLRUCache(lrucache* cache, char* phoneNumber) {
    if (cache->capacity == 0) {
        return; 
    }

    lrunode* newNode = (lrunode*)malloc(sizeof(lrunode));
    newNode->phonenumber = phoneNumber;
    newNode->prev = NULL;
    newNode->next = NULL;

    if (cache->count == 0) {
        cache->head = newNode;
        cache->tail = newNode;
        cache->count = 1;
    } else {
        newNode->next = cache->head;
        cache->head->prev = newNode;
        cache->head = newNode;

        if (cache->count < cache->capacity) {
            cache->count++;
        } else {
            lrunode* tailPrev = cache->tail->prev;
            tailPrev->next = NULL;
            free(cache->tail);
            cache->tail = tailPrev;
        }
    }
}
