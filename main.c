#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define MAX_CONTACTS 100
#define MAX_PHONE_NUMBER_LENGTH 15
#define CACHE_CAPACITY 5

typedef struct TrieNode
{
    char key;
    struct TrieNode *children[ALPHABET_SIZE];
    char *phone_number;
    int is_terminal;
} TrieNode;

typedef struct LRUCacheEntry
{
    char phone_number[MAX_PHONE_NUMBER_LENGTH];
    char contact_name[50];
    int search_frequency;
    struct LRUCacheEntry *prev;
    struct LRUCacheEntry *next;
} LRUCacheEntry;

TrieNode *root = NULL;
LRUCacheEntry *lru_cache_head = NULL;
LRUCacheEntry *lru_cache_tail = NULL;

TrieNode *create_trie_node(char key);
void insert_contact(char *phone_number, char *contact_name);
void search_contacts_helper(TrieNode *node, char *prefix, char **results, int *count);
void search_contacts(char *prefix, char **results);
void display_all_contacts(TrieNode *node, char *prefix);


LRUCacheEntry *create_cache_entry(char *phone_number, char *contact_name);
void add_to_cache(char *phone_number, char *contact_name);
void evict_lru_entry();
char *search_in_cache(char *contact_name);
void sort_cache_entries_by_frequency();
void display_frequently_searched_contacts();

void display_menu()
{
    printf("\nTelephone Directory:\n");
    printf("1. Add a contact\n");
    printf("2. Search contacts by name\n");
    printf("3. View all contacts\n");
    printf("4. View Frequently Searched contacts\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int get_alphabet_index(char c)
{
    return c - 'a';
}

TrieNode *create_trie_node(char key)
{
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (node)
    {
        node->key = key;
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            node->children[i] = NULL;
        }
        node->phone_number = NULL;
        node->is_terminal = 0;
    }
    return node;
}
void free_trie_nodes(TrieNode *node)
{
    if (node == NULL)
    {
        return;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        free_trie_nodes(node->children[i]);
    }
    free(node->phone_number);
    free(node);
}

void free_lru_cache_entries()
{
    LRUCacheEntry *entry = lru_cache_head;
    while (entry != NULL)
    {
        LRUCacheEntry *temp = entry;
        entry = entry->next;
        free(temp);
    }
}

void cleanup_memory()
{
    free_trie_nodes(root);
    free_lru_cache_entries();
}

void insert_contact(char *phone_number, char *contact_name)
{
    if (root == NULL)
    {
        root = create_trie_node('\0');
    }
    TrieNode *current = root;
    for (int i = 0; i < strlen(contact_name); i++)
    {
        char c = contact_name[i];
        int index = get_alphabet_index(c);
        if (current->children[index] == NULL)
        {
            current->children[index] = create_trie_node(c);
        }
        current = current->children[index];
    }
    if (current->phone_number != NULL)
    {
        free(current->phone_number);
    }
    current->phone_number = strdup(phone_number);
    current->is_terminal = 1;

    add_to_cache(phone_number, contact_name);
}

void search_contacts_helper(TrieNode *node, char *prefix, char **results, int *count)
{
    if (node == NULL)
    {
        return;
    }
    if (node->is_terminal)
    {
        results[(*count)++] = strdup(prefix);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->children[i] != NULL)
        {
            char next_prefix[50];
            sprintf(next_prefix, "%s%c", prefix, node->children[i]->key);
            search_contacts_helper(node->children[i], next_prefix, results, count);
        }
    }
}
int cache_size()
{
    int size = 0;
    LRUCacheEntry *entry = lru_cache_head;
    while (entry != NULL)
    {
        size++;
        entry = entry->next;
    }
    return size;
}

void search_contacts(char *prefix, char **results)
{
    TrieNode *current = root;
    int index = 0;
    while (index < strlen(prefix) && current != NULL)
    {
        int alphabet_index = get_alphabet_index(prefix[index]);
        current = current->children[alphabet_index];
        index++;
    }
    if (current == NULL)
    {
        return;
    }
    int count = 0;
    search_contacts_helper(current, prefix, results, &count);
}

void display_all_contacts(TrieNode *node, char *prefix)
{
    if (node == NULL)
    {
        return;
    }
    if (node->is_terminal)
    {
        printf("%s - %s\n", prefix, node->phone_number);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->children[i] != NULL)
        {
            char next_prefix[50];
            sprintf(next_prefix, "%s%c", prefix, node->children[i]->key);
            display_all_contacts(node->children[i], next_prefix);
        }
    }
}

LRUCacheEntry *create_cache_entry(char *phone_number, char *contact_name)
{
    LRUCacheEntry *entry = (LRUCacheEntry *)malloc(sizeof(LRUCacheEntry));
    if (entry)
    {
        strcpy(entry->phone_number, phone_number);
        strcpy(entry->contact_name, contact_name);
        entry->prev = NULL;
        entry->next = NULL;
    }
    return entry;
}

void add_to_cache(char *phone_number, char *contact_name)
{
    // Check if phone number already exists in cache and move it to the front (MRU position)
    LRUCacheEntry *entry = lru_cache_head;
    while (entry != NULL)
    {
        if (strcmp(entry->contact_name, contact_name) == 0)
        {
            // Move the found entry to the front (MRU position)
            if (entry != lru_cache_head)
            {
                if (entry == lru_cache_tail)
                {
                    lru_cache_tail = entry->prev;
                }
                entry->prev->next = entry->next;
                if (entry->next != NULL)
                {
                    entry->next->prev = entry->prev;
                }
                entry->next = lru_cache_head;
                entry->prev = NULL;
                lru_cache_head->prev = entry;
                lru_cache_head = entry;
            }
            return;
        }
        entry = entry->next;
    }

    // Create new cache entry and add to the front (MRU position)
    LRUCacheEntry *new_entry = create_cache_entry(phone_number, contact_name);
    if (new_entry == NULL)
    {
        return; // Memory allocation failed
    }

    new_entry->search_frequency = 0;

    if (lru_cache_head == NULL)
    {
        lru_cache_head = new_entry;
        lru_cache_tail = new_entry;
    }
    else
    {
        new_entry->next = lru_cache_head;
        lru_cache_head->prev = new_entry;
        lru_cache_head = new_entry;

        // Evict the LRU entry if cache size exceeds capacity
        if (cache_size() > CACHE_CAPACITY)
        {
            evict_lru_entry();
        }
    }
}

void evict_lru_entry()
{
    if (lru_cache_tail == NULL)
    {
        return;
    }

    // Remove the LRU entry (tail) from the cache
    if (lru_cache_tail == lru_cache_head)
    {
        free(lru_cache_tail);
        lru_cache_head = NULL;
        lru_cache_tail = NULL;
    }
    else
    {
        LRUCacheEntry *temp = lru_cache_tail;
        lru_cache_tail = lru_cache_tail->prev;
        lru_cache_tail->next = NULL;
        free(temp);
    }
}

char *search_in_cache(char *contact_name)
{
    // Search for phone number in the cache and return contact name if found
    LRUCacheEntry *entry = lru_cache_head;
    while (entry != NULL)
    {
        if (strcmp(entry->contact_name, contact_name) == 0)
        {
            // Move the found entry to the front (MRU position)
            entry->search_frequency++;
            if (entry != lru_cache_head)
            {
                if (entry == lru_cache_tail)
                {
                    lru_cache_tail = entry->prev;
                }
                entry->prev->next = entry->next;
                if (entry->next != NULL)
                {
                    entry->next->prev = entry->prev;
                }
                entry->next = lru_cache_head;
                entry->prev = NULL;
                lru_cache_head->prev = entry;
                lru_cache_head = entry;
            }
            return entry->contact_name;
        }
        entry = entry->next;
    }
    // If not found in cache, search in the Trie data structure
    TrieNode *current = root;
    for (int i = 0; i < strlen(contact_name); i++)
    {
        int index = get_alphabet_index(contact_name[i]);
        if (current->children[index] == NULL)
        {
            return NULL; // Contact name not found
        }
        current = current->children[index];
    }

    // Contact found in Trie, add it to the cache
    add_to_cache(current->phone_number, contact_name);

    // Return contact name along with phone number from Trie
    return current->phone_number;
}

void sort_cache_entries_by_frequency()
{
    // Bubble sort implementation
    int swapped;
    LRUCacheEntry *ptr1;
    LRUCacheEntry *lptr = NULL;

    // Check if cache is empty
    if (lru_cache_head == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = lru_cache_head;

        while (ptr1->next != lptr)
        {
            if (ptr1->search_frequency < ptr1->next->search_frequency)
            {
                // Swap the entries
                char temp_number[MAX_PHONE_NUMBER_LENGTH];
                char temp_name[50];
                int temp_frequency;

                strcpy(temp_number, ptr1->phone_number);
                strcpy(temp_name, ptr1->contact_name);
                temp_frequency = ptr1->search_frequency;

                strcpy(ptr1->phone_number, ptr1->next->phone_number);
                strcpy(ptr1->contact_name, ptr1->next->contact_name);
                ptr1->search_frequency = ptr1->next->search_frequency;

                strcpy(ptr1->next->phone_number, temp_number);
                strcpy(ptr1->next->contact_name, temp_name);
                ptr1->next->search_frequency = temp_frequency;

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    return;
}

void display_frequently_searched_contacts()
{
    // Sort cache entries based on search frequency
    sort_cache_entries_by_frequency();

    // Check if cache is empty
    if (lru_cache_head == NULL)
    {
        printf("No contacts found.\n");
        return;
    }

    // Limit the number of contacts to display
    int count = 0;
    LRUCacheEntry *entry = lru_cache_head;
    while (entry != NULL && count < CACHE_CAPACITY)
    {
        printf("%s - %s \n", entry->phone_number, entry->contact_name);
        entry = entry->next;
        count++;
    }
    return;
}

int main()
{
    int choice;
    do
    {
        display_menu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            char phone_number[MAX_PHONE_NUMBER_LENGTH];
            char contact_name[50];
            printf("Enter phone number: ");
            scanf("%s", phone_number);
            printf("Enter contact name: ");
            scanf("%s", contact_name);
            insert_contact(phone_number, contact_name);
            printf("Contact added successfully!\n");
            break;
        }
        case 2:
        {
            char prefix[50];
            printf("Enter prefix to search: ");
            scanf("%s", prefix);
            char *results[MAX_CONTACTS];
            for (int i = 0; i < MAX_CONTACTS; i++)
            {
                results[i] = NULL;
            }
            search_contacts(prefix, results);
            printf("Search results:\n");
            int found = 0; // Flag to check if any results were found
            for (int i = 0; i < MAX_CONTACTS && results[i] != NULL; i++)
            {
                char *contact_name = search_in_cache(results[i]);
                if (contact_name != NULL)
                {
                    // Retrieve phone number using the contact name
                    TrieNode *current = root;
                    for (int j = 0; j < strlen(results[i]); j++)
                    {
                        int index = get_alphabet_index(results[i][j]);
                        current = current->children[index];
                    }
                    printf("%s - %s\n", results[i], current->phone_number);
                    found = 1;
                }
            }
            if (!found)
            {
                printf("No results found.\n");
            }
            break;
        }
        case 3:
        {
            printf("All Contacts:\n");
            display_all_contacts(root, ""); // Call the display_all_contacts function
            break;
        }

        case 4:
        {
            printf("Frequently Searched Contacts:\n");
            display_frequently_searched_contacts();
            break;
        }
        case 5:
        {
            printf("Exiting\n");
            break;
        }
        default:
        {
            printf("Invalid choice. Please try again.\n");
            break;
        }
        }
    } while (choice != 5);

    cleanup_memory();
    printf("Cleared Lru Cache And Trie. \n");
    return 0;
}
