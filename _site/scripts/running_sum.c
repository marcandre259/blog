#include <stdlib.h>
#include <stdio.h>

int *runningSum(int *nums, int numsSize, int *returnSize)
{
    *returnSize = numsSize;
    int *return_arr = malloc(numsSize * sizeof(int));
    return_arr[0] = nums[0];

    for (int i = 1; i < numsSize; i++)
    {
        return_arr[i] = nums[i] + return_arr[i - 1];
    }

    return return_arr;
}

int main()
{
    int nums[] = {1, 2, 3, 4};
    int numsSize = sizeof(nums) / sizeof(nums[0]);
    int returnSize;

    int *output = runningSum(nums, numsSize, &returnSize);

    for (int i = 0; i < numsSize; i++)
    {
        printf("%d: %d\n", i, output[i]);
    }

    return 0;
}