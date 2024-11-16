#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

//unsigned int hash(const char *key) {
//    unsigned long int value = 0;
//    unsigned int i = 0;
//    unsigned int key_len = strlen(key);
//
//    for (; i < key_len; ++i) {
//        value = value * 37 + key[i];
//    }
//
//    value = value % TABLE_SIZE;
//
//    return value;
//}

static int hash(int key) {
    return key;
}

entry_t *ht_pair(int key, void *value) {
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = key;
    entry->value = value;
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void) {
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, int key, void *value) {
    int slot = hash(key);

    entry_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    while (entry != NULL) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }

        prev = entry;
        entry = prev->next;
    }

    prev->next = ht_pair(key, value);
}

void *ht_get(ht_t *hashtable, int key) {
    int slot = hash(key);

    entry_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        return NULL;
    }

    while (entry != NULL) {
        if (entry->key == key) {
            return entry->value;
        }

        entry = entry->next;
    }

    return NULL;
}

void ht_del(ht_t *hashtable, int key) {
    int bucket = hash(key);

    entry_t *entry = hashtable->entries[bucket];

    if (entry == NULL) {
        return;
    }

    entry_t *prev;
    int idx = 0;

    while (entry != NULL) {
        if (entry->key == key) {
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

            free(entry);
            return;
        }

        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

entry_t** ht_dump(ht_t *hashtable, int* count) {
    int size = 0;
    entry_t** entries = malloc(0);
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        for(;;) {
            entries = realloc(entries, sizeof(entry_t*) * (size + 1));
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

