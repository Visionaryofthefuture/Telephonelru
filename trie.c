#include<stdio.h>
#include<stdlib.h>

typedef struct trie
{
    char *phonenumber;
    struct trie* children[10]; // for the 10 digits in a phone number
} trie;

