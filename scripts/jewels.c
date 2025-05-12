#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 1000

int numJewelsInStones(char *jewels, char *stones)
{
    int *character_bank = calloc(TABLE_SIZE, sizeof(int));

    while (*jewels != '\0')
    {
        character_bank[*jewels] = 1;
        jewels++;
    }

    int cnt_jewels = 0;
    while (*stones != '\0')
    {
        cnt_jewels += character_bank[*stones];
        stones++;
    }

    return cnt_jewels;
}

int main()
{
    char *jewels = "z";
    char *stones = "ZZ";

    int output = numJewelsInStones(jewels, stones);

    printf("%d\n", output);
}