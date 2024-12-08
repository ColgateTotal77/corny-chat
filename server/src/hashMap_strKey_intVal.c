#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

static unsigned int hash(const char *key, int size) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    value = value % size;

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
    hashtable->size = TABLE_SIZE;
    hashtable->current_size = 0;

    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_str_set(ht_str_t *hashtable, char *key, int value) {
    int slot = hash(key, hashtable->size);

    entry_str_t *entry = hashtable->entries[slot];

    if (entry == NULL) {
        hashtable->entries[slot] = ht_str_pair(key, value);
        hashtable->current_size = 1 + hashtable->current_size;

        if (hashtable->current_size > hashtable->size) {
            enlarge_str_table(hashtable);
        }

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
    hashtable->current_size = 1 + hashtable->current_size;

    if (hashtable->current_size > hashtable->size) {
        enlarge_str_table(hashtable);
    }
}

int ht_str_get(ht_str_t *hashtable, char *key) {
    int slot = hash(key, hashtable->size);

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
    int bucket = hash(key, hashtable->size);

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

            hashtable->current_size = hashtable->current_size - 1;

            if (hashtable->current_size > TABLE_SIZE 
                && hashtable->current_size < (hashtable->size/4)) {
                shrink_str_table(hashtable);
            }

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
    for (int i = 0; i < hashtable->size; ++i) {
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

void delete_str_table(ht_str_t **hashtable) {
    int count = 0;
	entry_str_t **hash_slots = ht_str_dump(*hashtable, &count);

	for (int i = 0; i < count; i++) {
		entry_str_t *entry = hash_slots[i];
        free(entry->key);
		free(entry);
	}
    free(hash_slots);
	free((*hashtable)->entries);
	free(*hashtable);
    *hashtable = NULL;
}

void enlarge_str_table(ht_str_t *hashtable) {
    int old_entries_count = 0;
    entry_str_t** old_entries = ht_str_dump(hashtable, &old_entries_count);

    free(hashtable->entries);
    hashtable->size = 2 * hashtable->size;
    printf("Enlarging str hash table<-------%d----------------------%d--------------"
           "-------------\n", hashtable->current_size, hashtable->size);
    hashtable->current_size = 0;
    hashtable->entries = malloc(sizeof(entry_str_t*) * hashtable->size);

    for (int i = 0; i < hashtable->size; ++i) {
        hashtable->entries[i] = NULL;
    }

    for (int i = 0; i < old_entries_count; i++) {
        ht_str_set(hashtable, old_entries[i]->key, old_entries[i]->value);
        free(old_entries[i]->key);
        free(old_entries[i]);
    }
    free(old_entries);
}

void shrink_str_table(ht_str_t *hashtable) {
    int old_entries_count = 0;
    entry_str_t** old_entries = ht_str_dump(hashtable, &old_entries_count);

    free(hashtable->entries);
    hashtable->size = hashtable->size / 2;
    printf("Shrinking str hash table<-------%d----------------------%d-----------"
           "----------------\n", hashtable->current_size, hashtable->size);
    hashtable->current_size = 0;
    hashtable->entries = malloc(sizeof(entry_str_t*) * hashtable->size);

    for (int i = 0; i < hashtable->size; ++i) {
        hashtable->entries[i] = NULL;
    }

    for (int i = 0; i < old_entries_count; i++) {
        ht_str_set(hashtable, old_entries[i]->key, old_entries[i]->value);
        free(old_entries[i]->key);
        free(old_entries[i]);
    }
    free(old_entries);
}
