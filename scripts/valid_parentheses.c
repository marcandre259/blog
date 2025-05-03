#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

typedef struct List
{
    char *data;
    int tail;
} List;

List *initList(int n_chars)
{
    List *l = malloc(sizeof(List));
    l->data = malloc(n_chars * sizeof(char));
    l->tail = 0;
    return l;
}

void push_char(List *l, char c)
{
    int pos = l->tail;
    (l->data)[pos] = c;
    (l->tail)++;
}

char pop_char(List *l)
{
    if (l->tail == 0)
    {
        return '\0';
    }
    else
    {
        int pos = l->tail;
        char temp_char = l->data[pos];
        (l->tail)--;
        return temp_char;
    }
}

char peek_tail(List *l)
{
    int pos = (l->tail) - 1;
    char tail_char = (l->data)[pos];

    return tail_char;
}

void print_list(List *l)
{
    for (int i = 0; i < (l->tail); i++)
    {
        printf("%d: %c\n", i, l->data[i]);
    }
}

int which_char(char *s, int str_size, char c)
{
    for (int i = 0; i < str_size; i++)
    {
        if (c == s[i])
        {
            return i;
        }
    }
    return -1;
}

int main()
{
    char *input = "([{}])(){";
    int n = (int)strlen(input);

    char *open_chars = "([{";
    char *close_chars = ")]}";
    int n_open = (int)strlen(open_chars);

    // The list contains the open brackets
    List *l = initList(n);

    int output;

    for (int i = 0; i < n; i++)
    {
        int res_open = which_char(open_chars, n_open, input[i]);
        int res_close = which_char(close_chars, n_open, input[i]);

        if (res_open > -1)
        {
            push_char(l, input[i]);
        }
        else if (res_close > -1)
        {
            // Check if the found closing char correspond to
            // latest open char
            char peek_char = peek_tail(l);
            res_open = which_char(open_chars, n_open, peek_char);
            if (res_open == res_close)
            {
                char popped_char = pop_char(l);
            }
            else
            {
                output = 0;
                printf("%d\n", output);
                return 0;
            }
        }
    }

    if (l->tail > 0)
    {
        output = 0;
    }

    else
    {
        output = 1;
    }

    print_list(l);

    printf("%d\n", output);

    return 0;
}