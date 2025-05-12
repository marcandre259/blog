#include <stdio.h>
#include <stdlib.h>

#define NUM_SIZE 1001

int largestUniqueNumber(int *nums, int numsSize)
{
    int hash_table[NUM_SIZE] = {0};
    int n;

    for (int i = 0; i < numsSize; i++)
    {
        n = nums[i];
        hash_table[n]++;
    }

    for (int i = (NUM_SIZE - 1); i >= 0; i--)
    {
        if (hash_table[i] == 1)
        {
            return i;
        }
    }

    return -1;
}

int main()
{
    // int nums[] = {5, 7, 3, 9, 4, 9, 8, 3, 1};
    int nums[] = {9, 9, 8, 8};
    int numsSize = sizeof(nums) / sizeof(nums[0]);

    int output = largestUniqueNumber(nums, numsSize);

    printf("%d\n", output);

    return 0;
}