#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

static int hash(int key, int size) {
    return key % size;
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
    hashtable->size = TABLE_SIZE;
    hashtable->current_size = 0;

    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, int key, void *value) {
    int slot = hash(key, hashtable->size);

    entry_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        hashtable->current_size = 1 + hashtable->current_size;

        if (hashtable->current_size > hashtable->size) {
            enlarge_table(hashtable);
        }
        
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
    hashtable->current_size = 1 + hashtable->current_size;

    if (hashtable->current_size > hashtable->size) {
        enlarge_table(hashtable);
    }
}

void *ht_get(ht_t *hashtable, int key) {
    int slot = hash(key, hashtable->size);

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
    int bucket = hash(key, hashtable->size);

    printf("key %d. hash %d. curr_size %d. size %d\n", key, bucket, hashtable->current_size, hashtable->size);

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
            hashtable->current_size = hashtable->current_size - 1;

            if (hashtable->current_size > TABLE_SIZE && hashtable->current_size < (hashtable->size/4)) {
                shrink_table(hashtable);
            }

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
    for (int i = 0; i < hashtable->size; ++i) {
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

void enlarge_table(ht_t *hashtable) {
    int old_entries_count = 0;
    entry_t** old_entries = ht_dump(hashtable, &old_entries_count);

    free(hashtable->entries);
    hashtable->size = 2 * hashtable->size;
    printf("Enlarging hash table<-------%d----------------------%d--------------"
           "-------------\n", hashtable->current_size, hashtable->size);
    hashtable->current_size = 0;
    hashtable->entries = malloc(sizeof(entry_t*) * hashtable->size);

    for (int i = 0; i < hashtable->size; ++i) {
        hashtable->entries[i] = NULL;
    }

    for (int i = 0; i < old_entries_count; i++) {
        ht_set(hashtable, old_entries[i]->key, old_entries[i]->value);
        free(old_entries[i]);
    }
    free(old_entries);
}

void shrink_table(ht_t *hashtable) {
    
    int old_entries_count = 0;
    entry_t** old_entries = ht_dump(hashtable, &old_entries_count);

    free(hashtable->entries);
    hashtable->size = hashtable->size / 2;
    printf("Shrinking hash table<-------%d----------------------%d-----------"
           "----------------\n", hashtable->current_size, hashtable->size);
    hashtable->current_size = 0;
    hashtable->entries = malloc(sizeof(entry_t*) * hashtable->size);

    for (int i = 0; i < hashtable->size; ++i) {
        hashtable->entries[i] = NULL;
    }

    for (int i = 0; i < old_entries_count; i++) {
        ht_set(hashtable, old_entries[i]->key, old_entries[i]->value);
        free(old_entries[i]);
    }
    free(old_entries);
}

