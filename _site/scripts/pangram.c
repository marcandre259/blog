#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SET_SIZE 1007

typedef enum
{
    false,
    true
} bool;

typedef struct
{
    char **keys;
    int set_size;
} Set;

int hash_letter(char c)
{
    return (37 * c) % SET_SIZE;
}

Set *create_set()
{
    Set *set = malloc(sizeof(Set));
    set->keys = calloc(SET_SIZE, sizeof(char *));
    set->set_size = 0;

    return set;
}

void insert_key(char k, Set *s)
{
    int value = hash_letter(k);

    // Linear probing
    for (int i = 0; i < SET_SIZE; i++)
    {
        char *entry = s->keys[value];
        if (entry == NULL)
        {
            entry = malloc(sizeof(char));
            *entry = k;
            s->keys[value] = entry;
            s->set_size++;
            return;
        }
        else
        {
            if ((*entry) == k)
            {
                return;
            }
            else
            {
                value++;
                value = value % SET_SIZE;
            }
        }
    }
    return;
}

int get_set_size(Set *s)
{
    return s->set_size;
}

void traverse_set(Set *s)
{
    for (int i = 0; i < SET_SIZE; i++)
    {
        char *key = s->keys[i];
        if (key != NULL)
        {
            printf("%c", *key);
        }
    }
}

bool check_pangram(char *sentence)
{
    Set *set = create_set();
    int sentence_size = strlen(sentence);
    for (int i = 0; i < sentence_size; i++)
    {
        insert_key(sentence[i], set);
    }

    int set_size = get_set_size(set);
    if (set_size == 26)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    char *sentence = "thequic";

    bool output = check_pangram(sentence);

    if (output == true)
    {
        printf("true");
    }
    else
    {
        printf("false");
    }

    return 0;
}