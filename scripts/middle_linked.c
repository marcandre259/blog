#include <stdlib.h>
#include <stdio.h>

struct ListNode
{
    int val;
    struct ListNode *next;
};

struct ListNode *create_nodes(int *arr, int arrSize)
{
    struct ListNode *head = malloc(sizeof(struct ListNode));
    struct ListNode *dummy = head;
    for (int i = 0; i < arrSize; i++)
    {
        dummy->val = arr[i];
        if (i < (arrSize - 1))
        {
            dummy->next = malloc(sizeof(struct ListNode));
            dummy = dummy->next;
        }
        else
        {
            dummy->next = NULL;
        }
    }

    return head;
}

void traverse_nodes(struct ListNode *head)
{
    int cnt = 0;
    while (head != NULL)
    {
        printf("%d: %d\n", cnt, head->val);
        head = head->next;
        cnt++;
    }
}

struct ListNode *middleNode(struct ListNode *head)
{
    struct ListNode *slow = head;
    struct ListNode *fast = head;
    while (1)
    {
        if (fast->next == NULL)
        {
            return slow;
        }
        else if (fast->next->next == NULL)
        {
            return slow->next;
        }
        slow = slow->next;
        fast = fast->next->next;
    }
}

struct ListNode *deleteDuplicates(struct ListNode *head)
{
    struct ListNode *current = head;

    while (current != NULL && current->next != NULL)
    {
        if (current->next->val == current->val)
        {
            current->next = current->next->next;
        }
        else
        {
            current = current->next;
        }
    }
    return head;
}

struct ListNode *reverse(struct ListNode *head)
{
    struct ListNode *prev = NULL;

    while (head != NULL)
    {
        struct ListNode *new_node = head->next;
        head->next = prev;
        prev = head;
        head = new_node;
    }

    // Since head is NULL at the end
    return prev;
}

struct ListNode *reverseBetween(struct ListNode *head, int left, int right)
{
    if (head == NULL)
    {
        return NULL;
    }

    struct ListNode *current = head;
    struct ListNode *prev = NULL;
    struct ListNode *tail = NULL;
    struct ListNode *con = NULL;

    while (left > 1)
    {
        prev = current;
        current = current->next;
        right--;
        left--;
    }
    con = prev;
    prev = NULL;
    tail = current;
    while (right > 0)
    {
        struct ListNode *temp = current->next;
        current->next = prev;
        prev = current;
        current = temp;
        right--;
    }
    if (con)
    {
        con->next = prev;
    }
    else
    {
        head = prev;
    }
    tail->next = current;

    return head;
}

int main()
{
    int head[] = {1, 2, 3, 4, 5};
    int headSize = sizeof(head) / sizeof(head[0]);
    struct ListNode *h = create_nodes(head, headSize);

    // struct ListNode *r = reverse(h);

    int left = 2, right = 4;

    struct ListNode *hr = reverseBetween(h, left, right);

    traverse_nodes(hr);

    // struct ListNode *m = middleNode(h);

    // struct ListNode *u = deleteDuplicates(h);

    // traverse_nodes(u);

    return 0;
}