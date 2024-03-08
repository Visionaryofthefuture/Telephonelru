#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Trie Node
typedef struct TrieNode {
    struct TrieNode* children[10];  // Assuming phone numbers contain only digits (0-9)
    char* phoneNumber;
} TrieNode;

// LRU Cache Node
typedef struct LRUNode {
    struct LRUNode* prev;
    struct LRUNode* next;
    char* phoneNumber;
} LRUNode;

// LRU Cache
typedef struct LRUCache {
    int capacity;
    int count;
    LRUNode* head;
    LRUNode* tail;
} LRUCache;

// Function to initialize a TrieNode
TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node) {
        for (int i = 0; i < 10; ++i) {
            node->children[i] = NULL;
        }
        node->phoneNumber = NULL;
    }
    return node;
}

// Function to initialize an LRUCache
LRUCache* createLRUCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    if (cache) {
        cache->capacity = capacity;
        cache->count = 0;
        cache->head = NULL;
        cache->tail = NULL;
    }
    return cache;
}

// Function to insert a phone number into the Trie
void insertPhoneNumber(TrieNode* root, const char* phoneNumber, const char* contactName) {
    TrieNode* current = root;
    for (int i = 0; i < strlen(phoneNumber); ++i) {
        int index = phoneNumber[i] - '0';
        if (!current->children[index]) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    current->phoneNumber = strdup(contactName);
}

// Function to search for a phone number in the Trie
char* searchPhoneNumber(TrieNode* root, const char* phoneNumber) {
    TrieNode* current = root;
    for (int i = 0; i < strlen(phoneNumber); ++i) {
        int index = phoneNumber[i] - '0';
        if (!current->children[index]) {
            return NULL; // Phone number not found
        }
        current = current->children[index];
    }
    return current->phoneNumber;
}

// Function to update the LRU Cache
void updateLRUCache(LRUCache* cache, char* phoneNumber) {
    if (cache->capacity == 0) {
        return; // LRU cache disabled
    }

    LRUNode* newNode = (LRUNode*)malloc(sizeof(LRUNode));
    newNode->phoneNumber = phoneNumber;
    newNode->prev = NULL;
    newNode->next = NULL;

    if (cache->count == 0) {
        // Cache is empty
        cache->head = newNode;
        cache->tail = newNode;
        cache->count = 1;
    } else {
        // Cache is not empty
        newNode->next = cache->head;
        cache->head->prev = newNode;
        cache->head = newNode;

        if (cache->count < cache->capacity) {
            // Cache not yet at capacity
            cache->count++;
        } else {
            // Cache at capacity, remove the tail node
            LRUNode* tailPrev = cache->tail->prev;
            tailPrev->next = NULL;
            free(cache->tail);
            cache->tail = tailPrev;
        }
    }
}

// Function to display the Trie
void displayTrie(TrieNode* root, char* prefix) {
    if (root == NULL) {
        return;
    }
    if (root->phoneNumber != NULL) {
        printf("%s - %s\n", prefix, root->phoneNumber);
    }
    for (int i = 0; i < 10; ++i) {
        if (root->children[i] != NULL) {
            char newPrefix[strlen(prefix) + 2];
            sprintf(newPrefix, "%s%d", prefix, i);
            displayTrie(root->children[i], newPrefix);
        }
    }
}

// Function to display the LRU Cache
void displayLRUCache(LRUCache* cache) {
    printf("\nLRU Cache Contents:\n");
    LRUNode* current = cache->head;
    while (current != NULL) {
        printf("%s\n", current->phoneNumber);
        current = current->next;
    }
}

// Function to simulate the user interface
void userInterface(TrieNode* root, LRUCache* cache) {
    printf("Welcome to the Telephone Directory!\n");

    while (1) {
        char prefix[20];
        printf("\nEnter a prefix to search for contacts (or 'exit' to quit): ");
        scanf("%s", prefix);

        if (strcmp(prefix, "exit") == 0) {
            break;
        }

        printf("Searching for contacts with prefix '%s'...\n", prefix);

        char* result = searchPhoneNumber(root, prefix);

        if (result != NULL) {
            printf("Results:\n%s - %s\n", prefix, result);
            updateLRUCache(cache, result);
        } else {
            printf("No contacts found with the given prefix.\n");
        }

        displayLRUCache(cache);
    }

    printf("Exiting the Telephone Directory. Goodbye!\n");
}

// Main function for testing
int main() {
    TrieNode* root = createTrieNode();
    LRUCache* cache = createLRUCache(3);

    insertPhoneNumber(root, "123", "Alice");
    insertPhoneNumber(root, "987", "Andrew");
    insertPhoneNumber(root, "555", "Alan");

    displayTrie(root, "");

    userInterface(root, cache);

    return 0;
}
