#include <stdlib.h>
#include <stdio.h>

int *getAverages(int *nums, int numsSize, int k, int *returnSize)
{
    // compute cum sum
    double *cum_sum = (double *)malloc(numsSize * sizeof(double));
    cum_sum[0] = nums[0];

    *returnSize = numsSize;

    for (int i = 1; i < numsSize; i++)
    {
        cum_sum[i] = nums[i] + cum_sum[i - 1];
    }

    int *k_radius_avgs = (int *)malloc(numsSize * sizeof(int));
    int left, right;

    // 2 * k + 1 is the normalizing constant
    double norm_constant = 1 / (2 * (double)k + 1);

    for (int i = 0; i < numsSize; i++)
    {
        left = i - k;
        right = i + k;
        if (left < 0 || right > (numsSize - 1))
        {
            k_radius_avgs[i] = -1;
        }
        else
        {
            if (left == 0)
            {
                k_radius_avgs[i] = (int)(cum_sum[right] * norm_constant);
            }
            else
            {
                k_radius_avgs[i] = (int)((cum_sum[right] - cum_sum[left - 1]) * norm_constant);
            }
        }
    }

    free(cum_sum);

    return k_radius_avgs;
}

int main()
{
    int nums[] = {7, 4, 3, 9, 1, 8, 5, 2, 6};
    int numsSize = sizeof(nums) / sizeof(nums[0]);
    int returnSize;
    int k = 3;

    int *output = getAverages(nums, numsSize, k, &returnSize);

    for (int i = 0; i < numsSize; i++)
    {
        printf("%d: %d\n", i, output[i]);
    }

    free(output);

    return 0;
}