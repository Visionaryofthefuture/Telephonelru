#include<stdio.h>
#include<stdlib.h>

typedef struct trie
{
    char *phonenumber;
    struct trie* children[10]; // for the 10 digits in a phone number
} trie;

/* Trie Structure:
    0
    |
    1
  / / | \ \ 
 1 2  3  4 5 and so on
*/

