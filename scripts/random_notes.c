#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 1000

typedef enum
{
    false,
    true
} bool;

bool canConstruct(char *ransomNote, char *magazine)
{
    int **character_bank = malloc(TABLE_SIZE * sizeof(int *));
    int array_size = 2;

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        character_bank[i] = calloc(array_size, sizeof(int));
    }

    int key;
    char *ptr_ransomNote = ransomNote;

    while (*ptr_ransomNote != '\0')
    {
        key = (int)(*ptr_ransomNote);
        character_bank[key][0]++;

        ptr_ransomNote++;
    }

    char *ptr_magazine = magazine;

    while (*ptr_magazine != '\0')
    {
        key = (int)(*ptr_magazine);
        character_bank[key][1]++;

        ptr_magazine++;
    }

    // Now check
    while (*ransomNote != '\0')
    {
        key = (int)(*ransomNote);
        if (character_bank[key][0] > character_bank[key][1])
        {
            bool output = false;

            // free memory and return
            for (int i = 0; i < TABLE_SIZE; i++)
            {
                free(character_bank[i]);
            }
            free(character_bank);

            return output;
        }

        ransomNote++;
    }

    // free memory and return
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        free(character_bank[i]);
    }
    free(character_bank);

    bool output = true;

    return output;
}

int main()
{

    char *ransomNote = "boat";
    char *magazine = "abot";

    bool output = canConstruct(ransomNote, magazine);
    if (output == true)
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }

    return 0;
}