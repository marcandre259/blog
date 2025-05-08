#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 10000

typedef struct Entry
{
    char *key;
    int value;
    struct Entry *next;
} Entry;

typedef struct HashTable
{
    Entry **entries;
} HashTable;

int hash(char *key);
void insert_entry(char *key, int value, HashTable *ht);
int get_entry(char *key, HashTable *ht);
HashTable *create_table();
void traverse_ht(HashTable *ht);

#endif // HASH_MAP_H
