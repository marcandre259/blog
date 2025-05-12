#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 256

int lengthOfLongestSubstring(char *s)
{
    int *repeat_arr = malloc(TABLE_SIZE * sizeof(int));

    // Initialize at -1
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        repeat_arr[i] = -1;
    }

    int max_length = 0;
    int current_length;
    int right = 0;
    int s_length = strlen(s);
    int key;
    for (int i = 0; i < s_length; i++)
    {
        key = (int)(s[i]);
        if (repeat_arr[key] != -1)
        {
            right = repeat_arr[key] + 1;
            // Roll back location in hash map less than i
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                if (repeat_arr[j] < repeat_arr[key])
                {
                    repeat_arr[j] = -1;
                }
            }
        }
        repeat_arr[key] = i;
        current_length = (i + 1) - right;

        if (current_length > max_length)
        {
            max_length = current_length;
        }
    }

    free(repeat_arr);

    return max_length;
}

int main()
{
    char *s = "abcabcbb";
    // char *s = "bbtablud";

    int output = lengthOfLongestSubstring(s);

    printf("%d\n", output);
    return 0;
}