#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

static unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    value = value % TABLE_SIZE;

    return value;
}

entry_str_t *ht_str_pair(char* key, int value) {
    entry_str_t *entry = malloc(sizeof(entry_str_t) * 1);
    entry->key = (char*)malloc(strlen(key) + 1);
    strcpy(entry->key, key);
    entry->value = value;
    entry->next = NULL;

    return entry;
}

ht_str_t *ht_str_create(void) {
    ht_str_t *hashtable = malloc(sizeof(ht_str_t) * 1);
    hashtable->entries = malloc(sizeof(entry_str_t*) * TABLE_SIZE);

    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_str_set(ht_str_t *hashtable, char *key, int value) {
    int slot = hash(key);

    entry_str_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        hashtable->entries[slot] = ht_str_pair(key, value);
        return;
    }

    entry_str_t *prev;

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }

        prev = entry;
        entry = prev->next;
    }

    prev->next = ht_str_pair(key, value);
}

int ht_str_get(ht_str_t *hashtable, char *key) {
    int slot = hash(key);

    entry_str_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        return -1;
    }

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        entry = entry->next;
    }

    return -1;
}

void ht_str_del(ht_str_t *hashtable, char *key) {
    int bucket = hash(key);

    entry_str_t *entry = hashtable->entries[bucket];

    if (entry == NULL) {
        return;
    }

    entry_str_t *prev;
    int idx = 0;

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            free(entry->key);
            free(entry);
            return;
        }

        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

entry_str_t** ht_str_dump(ht_str_t *hashtable, int* count) {
    int size = 0;
    entry_str_t** entries = malloc(0);
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_str_t *entry = hashtable->entries[i];
        
        if (entry == NULL) {
            continue;
        }

        for(;;) {
            entries = realloc(entries, sizeof(entry_str_t*) * (size + 1));
            entries[size] = entry;
            size++;

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }
    }
    *count = size;
    return entries;
}

