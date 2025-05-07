#include <stdlib.h>
#include <stdio.h>

#define SET_SIZE 10000

typedef struct Entry
{
    int value;
    struct Entry *next;
} Entry;

typedef struct
{
    Entry **entries;
} Set;

Set *create_set()
{
    Set *set = malloc(sizeof(Set));
    set->entries = (Entry **)calloc(SET_SIZE, sizeof(Entry *));

    return set;
}

int hash(int value)
{
    return value % SET_SIZE;
}

void insert_entry(int value, Set *set)
{
    int hash_score = hash(value);

    Entry *entry = set->entries[hash_score];

    if (entry == NULL)
    {
        Entry *new_entry = malloc(sizeof(Entry));
        new_entry->value = value;
        new_entry->next = NULL;

        set->entries[hash_score] = new_entry;
        return;
    }
    else
    {
        if ((entry->value) == value)
        {
            return;
        }
        else
            while (entry->next != NULL)
            {
                if ((entry->next->value) == value)
                {
                    return;
                }
                entry = entry->next;
            }

        Entry *new_entry = malloc(sizeof(Entry));
        new_entry->value = value;
        new_entry->next = NULL;
        entry->next = new_entry;
        return;
    }
}

int get_entry(int value, Set *set)
{
    int hash_score = hash(value);
    Entry *entry = set->entries[hash_score];

    if (entry == NULL)
    {
        return -1;
    }
    else
    {
        while (entry != NULL)
        {
            int curr_value = entry->value;
            if (curr_value == value)
            {
                return value;
            }
            entry = entry->next;
        }
        return -1;
    }
}

int countElements(int *arr, int arrSize)
{
    Set *set = create_set();
    int cnt = 0;

    for (int i = 0; i < arrSize; i++)
    {
        insert_entry(arr[i], set);
    }

    for (int i = 0; i < arrSize; i++)
    {
        int curr_value = arr[i];
        int curr_output = get_entry(curr_value + 1, set);
        if (curr_output > -1)
        {
            cnt++;
        }
    }

    return cnt;
}

int main()
{
    // int arr[] = {1, 2, 3};
    int arr[] = {1, 1, 3, 3, 5, 5, 6, 7, 7};
    int arrSize = sizeof(arr) / sizeof(arr[0]);

    int cnt = countElements(arr, arrSize);

    printf("%d\n", cnt);

    return 0;
}