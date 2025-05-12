#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE 100

char *makeGood(char *s)
{
    char *stack = (char *)malloc(STACK_SIZE * sizeof(char) + 1);
    memset(stack, 0, STACK_SIZE + 1);
    int stack_idx = 0;
    int check_diff;

    while (*s != '\0')
    {
        if (stack_idx > 0)
        {
            check_diff = (*s - stack[stack_idx - 1]);
        }
        else
        {
            check_diff = 0;
        }
        if (check_diff == 32 || check_diff == -32)
        {
            stack[stack_idx - 1] = '\0';
            stack_idx--;
        }
        else
        {
            stack[stack_idx] = *s;
            stack_idx++;
        }
        s++;
    }
    return stack;
}

int main()
{
    char *s = "leEeetcode";
    // char *s = "abBAcC";

    int out = 'e' - 'E';
    char *output = makeGood(s);

    printf("%s\n", output);

    // printf("%d\n", out);

    return 0;
}