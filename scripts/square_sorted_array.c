#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int *sortedSquares(int *nums, int numsSize, int *returnSize)
{
    int left_pos = 0;
    int right_pos = numsSize - 1;
    int pos = numsSize - 1;

    int *output = (int *)malloc(numsSize * sizeof(int));
    *returnSize = numsSize;

    while (left_pos < right_pos)
    {
        int left_num = nums[left_pos] * nums[left_pos];
        int right_num = nums[right_pos] * nums[right_pos];

        if (left_num > right_num)
        {
            output[pos] = left_num;
            left_pos++;
        }
        else
        {
            output[pos] = right_num;
            right_pos--;
        }
        pos--;
    }
    output[0] = nums[left_pos] * nums[left_pos];

    return output;
}

int main()
{
    int arr[] = {-9, -4, -1, 0, 3, 10, 10};
    int n = sizeof(arr) / sizeof(arr[0]);

    int *output = sortedSquares(arr, n, &n);

    for (int i = 0; i < n; i++)
    {
        printf("%d: %d\n", i, output[i]);
    }

    free(output);

    return 0;
}