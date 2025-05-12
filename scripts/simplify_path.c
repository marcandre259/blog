#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE 1000
#define PATH_SIZE 3000

char *simplifyPath(char *path)
{
    char *stack[STACK_SIZE];
    char *token = strtok(path, "/");
    int stack_idx = 0;

    while (token != NULL)
    {
        if (strcmp(token, ".") == 0 || strcmp(token, "") == 0)
        {
            token = strtok(NULL, "/");
            continue;
        }
        else if (strcmp(token, "..") == 0)
        {
            if (stack_idx > 0)
            {
                stack_idx--;
            }
            token = strtok(NULL, "/");
            continue;
        }
        else
        {
            stack[stack_idx] = token;
            stack_idx++;
        }
        token = strtok(NULL, "/");
    }

    char *simple_path = (char *)malloc(PATH_SIZE * sizeof(char));
    memset(simple_path, 0, PATH_SIZE);

    for (int i = 0; i < stack_idx; i++)
    {
        simple_path = strcat(simple_path, "/");
        simple_path = strcat(simple_path, stack[i]);
    }

    if (strcmp(simple_path, "") == 0)
    {
        return "/";
    }

    return simple_path;
}

int main()
{
    char path[] = "/home/user/Documents/../../../../../Pictures/../";

    char *output = simplifyPath(path);

    printf("%s\n", output);

    return 0;
}
