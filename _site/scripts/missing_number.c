#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    int *values;
    int set_size;
} Set;

Set *create_set(int numsSize)
{
    Set *s = malloc(sizeof(Set));
    s->set_size = numsSize + 1;
    s->values = malloc((s->set_size) * sizeof(int));
    for (int i = 0; i < (s->set_size); i++)
    {
        s->values[i] = -1;
    }

    return s;
}

void insert_value(int value, Set *s)
{
    s->values[value] = value;
}

int get_missing_value(Set *s)
{
    for (int i = 0; i < (s->set_size); i++)
    {
        if (s->values[i] == -1)
        {
            return i;
        }
    }

    return -1;
}

int missing_number(int *nums, int numsSize)
{
    Set *set = create_set(numsSize);
    for (int i = 0; i < numsSize; i++)
    {
        insert_value(nums[i], set);
    }

    int res = get_missing_value(set);

    return res;
}

int main()
{
    int nums[] = {9, 6, 4, 2, 3, 5, 7, 0, 1};
    int numsSize = sizeof(nums) / sizeof(nums[0]);

    Set *set = create_set(numsSize);
    for (int i = 0; i < numsSize; i++)
    {
        insert_value(nums[i], set);
    }

    int output = get_missing_value(set);

    printf("%d\n", output);

    return 0;
}