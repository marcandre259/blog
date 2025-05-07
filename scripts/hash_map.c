#include "hash_map.h"

int hash(char *key)
{
    int value = 0;
    int key_len = strlen(key);
    for (int i = 0; i < key_len; i++)
    {
        value = 37 * value + key[i];
    }

    value = value % TABLE_SIZE;

    return value;
}

void insert_entry(char *key, int value, HashTable *ht)
{
    int idx = hash(key);

    Entry *new_entry = malloc(sizeof(Entry));

    new_entry->key = malloc(strlen(key) * sizeof(char));
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = NULL;

    Entry *entry = ht->entries[idx];
    if (entry == NULL)
    {
        ht->entries[idx] = new_entry;
        return;
    }

    while (entry->next != NULL)
    {
        entry = entry->next;
    }

    entry->next = new_entry;
    return;
}

int get_entry(char *key, HashTable *ht)
{
    int idx = hash(key);

    Entry *entry = ht->entries[idx];
    if (entry == NULL)
    {
        return -1;
    }

    if (strcmp(entry->key, key) == 0)
    {
        return entry->value;
    }
    while (entry->next != NULL)
    {
        entry = entry->next;
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }
    }

    return -1;
}

HashTable *create_table()
{
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->entries = malloc(TABLE_SIZE * sizeof(Entry *));

    return hash_table;
}

void traverse_ht(HashTable *ht)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Entry *entry = ht->entries[i];
        if (entry != NULL)
        {
            printf("Key: %s, ", entry->key);
            printf("Value: %d\n", entry->value);
        }
    }
}

int main()
{
    HashTable *ht = create_table();

    char *k = "a";
    int v = 2;

    char *k2 = "b";
    int v2 = 5;

    insert_entry(k, v, ht);
    insert_entry(k2, v2, ht);

    traverse_ht(ht);

    return 0;
}
