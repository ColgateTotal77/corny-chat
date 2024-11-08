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

typedef struct entry_str_t {
    char* key;
    int value;
    struct entry_str_t *next;
} entry_str_t;

typedef struct {
    entry_str_t **entries;
} ht_str_t;

ht_str_t *ht_str_create(void);
void ht_str_set(ht_str_t *hashtable, char* key, int value);
int ht_str_get(ht_str_t *hashtable, char* key);
void ht_str_del(ht_str_t *hashtable, char* key);
entry_str_t** ht_str_dump(ht_str_t *hashtable, int* count);

