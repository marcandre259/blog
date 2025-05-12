#include <stdio.h>
#include <stdlib.h>

int findMaxLength(int *nums, int numsSize)
{
    int *diff_array = malloc((2 * numsSize + 1) * sizeof(int));
    for (int i = 0; i < (2 * numsSize + 1); i++)
    {
        diff_array[i] = -1;
    }
    int max_length = 0;
    int current_length;
    int cnt_ones = 0;
    int cnt_zeros = 0;
    int current_diff;
    int key;
    printf("\nInitial at 12: %d\n", diff_array[11]);

    for (int i = 0; i < numsSize; i++)
    {
        if (nums[i] == 0)
        {
            cnt_zeros++;
        }
        else
        {
            cnt_ones++;
        }
        current_diff = cnt_ones - cnt_zeros;
        printf("\n-------\n");
        printf("current diff: %d\n", current_diff);

        key = (current_diff + numsSize);
        printf("key: %d\n", key);
        if (diff_array[key] > i || diff_array[key] == -1)
        {
            printf("previous value: %d\n", diff_array[key]);
            printf("i: %d\n", i);
            diff_array[key] = i;
        }

        if (current_diff == 0)
        {
            if (max_length < (i + 1))
            {
                max_length = i + 1;
            }
        }
        else if (diff_array[key] != -1)
        {
            printf("diff array at key: %d\n", diff_array[key]);
            current_length = i - diff_array[key];
            if (max_length < current_length)
            {
                max_length = current_length;
            }
        }
    }
    return max_length;
}

int main()
{
    // int nums[] = {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    // int nums[] = {0, 0, 0};
    int nums[] = {1, 1, 1, 1, 1, 1};
    int numsSize = sizeof(nums) / sizeof(nums[0]);

    int output = findMaxLength(nums, numsSize);

    printf("%d\n", output);

    return 0;
}