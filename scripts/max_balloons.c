#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node
{
    char c;
    float cnt;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    Node *root;
} LinkedList;

LinkedList *create_list(char *text)
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->root = malloc(sizeof(Node));
    Node *current_node = list->root;

    int n_chars = strlen(text);

    for (int i = 0; i < n_chars; i++)
    {
        current_node->c = text[i];
        current_node->cnt = 0;
        current_node->next = NULL;

        if (i < (n_chars - 1))
        {
            current_node->next = malloc(sizeof(Node));
            current_node = current_node->next;
        }
    }

    return list;
}

void push_char(char c, LinkedList *l)
{
    Node *current_node = l->root;

    while (current_node != NULL)
    {
        if (current_node->c == c)
        {
            if (c == 'o' || c == 'l')
            {
                current_node->cnt = current_node->cnt + 0.5;
            }
            else
            {
                current_node->cnt = current_node->cnt + 1;
            }
        }
        current_node = current_node->next;
    }
}

void traverse_list(LinkedList *l)
{
    Node *current_node = l->root;
    int cnt = 0;
    printf("%d: %c, %.2f\n", cnt, current_node->c, current_node->cnt);
    while (current_node->next != NULL)
    {
        cnt++;
        current_node = current_node->next;
        printf("%d: %c, %.2f\n", cnt, current_node->c, current_node->cnt);
    }
}

int maxNumberOfBalloons(char *text)
{
    char *word = "balon";
    int text_length = strlen(text);
    LinkedList *ref_list = create_list(word);

    for (int i = 0; i < text_length; i++)
    {
        push_char(text[i], ref_list);
    }

    int min_total = -1;
    Node *current_node = ref_list->root;
    while (current_node != NULL)
    {
        if (min_total == -1)
        {
            min_total = current_node->cnt;
        }
        else
        {
            if (current_node->cnt < min_total)
            {
                min_total = current_node->cnt;
            }
        }
        current_node = current_node->next;
    }

    // Could be 0.5 for example
    if (min_total < 1)
    {
        min_total = 0;
    }

    return min_total;
}

int main()
{
    // char *text = "nlaebolko";
    char *text = "loonbalxballpoon";

    char *word = "balon";

    LinkedList *list = create_list(word);

    traverse_list(list);

    int output = maxNumberOfBalloons(text);

    printf("Balloon count: %d\n", output);

    return 0;
}