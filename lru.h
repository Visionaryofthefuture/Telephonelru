#include<stdio.h>
#include<stdlib.h>

typedef struct lrunode
{
    struct lrunode *prev;
    struct lrunode *next;
    char *phonenumber;
}lrunode;

typedef struct lrucache
{
    int capacity, count;
    lrunode *head;
    lrunode *tail;

} lrucache;

