#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void reverse_string(char *string, int string_size)
{
    for (int i = 0; i < string_size; i++)
    {
        int pos = string_size - i - 1;
        if (pos <= i)
        {
            break;
        }
        char temp_char = string[i];
        string[i] = string[pos];
        string[pos] = temp_char;
    }
}

int main()
{
    char input[] = "Katrien is cute";
    int input_size = strlen(input);

    reverse_string(input, input_size);

    printf("Input: %s\n", input);
}
