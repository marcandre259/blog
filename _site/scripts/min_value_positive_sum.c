#include <stdlib.h>
#include <stdio.h>

int minStartValue(int *nums, int numsSize)
{
    int min_sum = nums[0];
    int cum_sum = 0;
    int start_value;

    for (int i = 0; i < numsSize; i++)
    {
        cum_sum += nums[i];
        if (cum_sum < min_sum)
        {
            min_sum = cum_sum;
        }
    }

    if (min_sum < 1)
    {
        start_value = -1 * min_sum + 1;
    }
    else
    {
        start_value = 1;
    }

    return start_value;
}

int main()
{
    // int nums[] = {-3, 2, -3, 4, 2};
    int nums[] = {1, -2, -3};
    int numsSize = sizeof(nums) / sizeof(nums[0]);

    int output = minStartValue(nums, numsSize);

    printf("%d\n", output);

    return 0;
}