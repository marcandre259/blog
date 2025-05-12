#include <stdlib.h>
#include <stdio.h>

struct Node
{
    int val;
    struct Node *prev;
    struct Node *next;
};

typedef struct Queue
{
    int len;
    struct Node *head;
    struct Node *tail;
} Queue;

Queue *create_queue()
{
    Queue *queue = malloc(sizeof(Queue));
    queue->len = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void enqueue(int val, Queue *q)
{
    if (q->tail == NULL)
    {
        struct Node *node = malloc(sizeof(struct Node));

        node->val = val;
        node->prev = NULL;
        node->next = NULL;

        // Same pointer for head and tail initially
        q->head = node;
        q->tail = node;
    }
    else
    {
        struct Node *prev = q->tail;
        struct Node *current = malloc(sizeof(struct Node));
        current->val = val;
        current->prev = prev;
        current->next = NULL;
        prev->next = current;
        q->tail = current;
    }
    q->len++;
}

void dequeue(Queue *q)
{
    if (q->head == NULL)
    {
        return;
    }
    struct Node *prev = q->head;
    struct Node *current = prev->next;
    free(prev);
    if (current == NULL)
    {
        q->head = NULL;
        return;
    }
    current->prev = NULL;
    q->head = current;
    q->len--;
}

void traverse_queue(Queue *q)
{
    struct Node *node = q->head;
    while (node != NULL)
    {
        printf("%d\n", node->val);
        node = node->next;
    }
}

typedef struct
{
    int max_len;
    Queue *queue;
} MovingAverage;

MovingAverage *movingAverageCreate(int size)
{
    MovingAverage *moving_average = malloc(sizeof(MovingAverage));
    Queue *queue = create_queue();

    moving_average->queue = queue;
    moving_average->max_len = size;

    return moving_average;
}

double movingAverageNext(MovingAverage *obj, int val)
{
    Queue *queue = obj->queue;
    int max_len = obj->max_len;
    enqueue(val, queue);
    if (queue->len > max_len)
    {
        dequeue(queue);
    }

    double sum = 0;
    double norm = (double)queue->len;
    struct Node *current = queue->head;
    while (current != NULL)
    {
        sum += (double)current->val;
        current = current->next;
    }

    double avg = sum / norm;

    return avg;
}

void movingAverageFree(MovingAverage *obj)
{
    Queue *queue = obj->queue;
    struct Node *current = queue->head;
    while (current != NULL)
    {
        struct Node *temp = current->next;
        free(current);
        current = temp;
    }
    free(queue);
    free(obj);
}

/**
 * Your MovingAverage struct will be instantiated and called as such:
 * MovingAverage* obj = movingAverageCreate(size);
 * double param_1 = movingAverageNext(obj, val);

 * movingAverageFree(obj);
*/

int main()
{
    MovingAverage *obj = movingAverageCreate(3);
    for (int i = 1; i < 6; i++)
    {
        double output = movingAverageNext(obj, i);
        printf("%.2f\n", output);
    }

    movingAverageFree(obj);

    return 0;
}