#include <stdlib.h>
#include <stdio.h>

int max(int x, int y)
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

int longestOnes(int *nums, int numsSize, int k)
{
    int right = 0;
    int cnt = 0;
    int score = 0;
    int curr = 0;

    for (int i = 0; i < numsSize; i++)
    {
        while ((right < numsSize))
        {
            if (nums[right] == 0)
            {
                cnt++;
            }
            if (cnt > k)
            {
                cnt--;
                break;
            }
            curr++;
            right++;
        }
        printf("Current score: %d, at: %d", curr, i);
        score = max(score, curr);

        printf(", Count zeros: %d, Right pos: %d\n", cnt, right);
        curr--;

        if (nums[i] == 0)
        {
            cnt--;
        }
    }

    return score;
}

int main()
{
    int nums[] = {1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0};
    int numsSize = sizeof(nums) / sizeof(nums[0]);
    int k = 2;

    int output = longestOnes(nums, numsSize, k);

    printf("%d\n", output);
}