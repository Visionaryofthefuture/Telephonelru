#include"trie.h"
#include<stdlib.h>
#include<string.h>
trie * init()
{
    trie *root = (trie *)malloc(sizeof(trie));
    if(!root)
    {
        return root;
    }

    for(int i =0 ; i<10; ++i)
    {
        root->children[i] = NULL;
    }
    root->phonenumber = NULL;
}

void insertingphonenumber(trie *root, char *phonenumber, char *name)
{
    trie *curr = root;
    if(strlen(phonenumber)< 10)
    {
        printf("Please input 10 digit phone number ! \n");
        return;
    }
    for(int i = 0; i <10; ++i)
    {
        int index = phonenumber[i] - '0';
        if(!curr->children[index])
        {
            curr->children[index] = init();
        }
        curr = curr->children[index];
    }

    strcpy(curr->phonenumber, phonenumber);
}

char* searchPhoneNumber(trie* root, const char* phoneNumber) 
{
    trie* current = root;
    for (int i = 0; i < strlen(phoneNumber); ++i) 
    {
        int index = phoneNumber[i] - '0';
        if (!current->children[index])
        {
            return NULL; 
        }
        current = current->children[index];
    }
    return current->phonenumber;
}