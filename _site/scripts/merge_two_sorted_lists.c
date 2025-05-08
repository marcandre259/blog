#include <stdlib.h>
#include <stdio.h>

typedef struct LinkedNode
{
    int val;
    struct LinkedNode *next;
} LinkedNode;

LinkedNode *merge_sorted_lists(int *arr_a, int a_size, int *arr_b, int b_size)
{
    int pos_a = 0;
    int pos_b = 0;

    LinkedNode *root_node = malloc(sizeof(LinkedNode));
    LinkedNode *previous_node = root_node;

    // Strategy is to keep track of the root_node's address
    // So we can traverse from it
    while ((pos_a < a_size) | (pos_b < b_size))
    {
        LinkedNode *new_node = malloc(sizeof(LinkedNode));
        previous_node->next = new_node;

        if (pos_a >= a_size)
        {
            new_node->val = arr_b[pos_b];
            pos_b++;
        }
        else if (pos_b >= b_size)
        {
            new_node->val = arr_a[pos_a];
            pos_a++;
        }
        else
        {
            int val_a = arr_a[pos_a];
            int val_b = arr_b[pos_b];

            if (val_a < val_b)
            {
                new_node->val = arr_a[pos_a];
                pos_a++;
            }
            else
            {
                new_node->val = arr_b[pos_b];
                pos_b++;
            }
        }

        previous_node = new_node;
    }

    return root_node;
}

void RootTraversal(LinkedNode *l)
{
    LinkedNode *current_node = l;
    int cnt = 0;

    while (current_node->next != NULL)
    {
        current_node = current_node->next;
        printf("%d: %d\n", cnt, current_node->val);
        cnt++;
    }
}

void free_list(LinkedNode *l)
{
    while ()
}

// Add freeing logic

int main()
{
    int arr_a[] = {1, 5, 9};
    int arr_b[] = {2, 3};

    int n_a = sizeof(arr_a) / sizeof(arr_a[0]);
    int n_b = sizeof(arr_b) / sizeof(arr_b[0]);

    LinkedNode *root_node = merge_sorted_lists(arr_a, n_a, arr_b, n_b);

    RootTraversal(root_node);
}