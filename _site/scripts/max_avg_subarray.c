#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double max(double x, double y)
{
    if (x > y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

double findMaxAverage(int *nums, int numsSize, int k)
{
    int right = 0;
    double curr = 0;
    double res;

    // Initialize
    for (int i = 0; i < k; i++)
    {
        res += (1.0 / (double)k) * (double)(nums[i]);
    }

    for (int left = 0; left < (numsSize - k + 1); left++)
    {
        while ((right - left) < k)
        {
            curr += (1.0 / (double)k) * (double)nums[right];
            right++;
        }

        res = max(curr, res);
        curr -= (1.0 / (double)k) * (double)nums[left];
    }

    return res;
}

int main()
{
    // int nums[] = {1, 12, -5, -6, 50, 3};
    int nums[] = {-1};
    int nums_size = sizeof(nums) / sizeof(nums[0]);
    // int k = 4;
    int k = 1;

    double output = findMaxAverage(nums, nums_size, k);

    printf("Output: %.3f\n", output);

    return 0;
}