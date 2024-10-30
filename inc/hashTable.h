#pragma once

#define TABLE_SIZE 200

typedef struct entry_t {
    int key;
    void *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;

ht_t *ht_create(void);
void ht_set(ht_t *hashtable, int key, void *value);
void *ht_get(ht_t *hashtable, int key);
void ht_del(ht_t *hashtable, int key);
entry_t** ht_dump(ht_t *hashtable, int* count);


