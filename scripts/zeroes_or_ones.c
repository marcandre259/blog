#include <stdlib.h>
#include <stdio.h>

int **findWinners(int **matches, int matchesSize, int *matchesColSize, int *returnSize, int **returnColumnSizes)
{
}

int main()
{
    int *win_teams = {2, 1, 5, 6};
    int *loss_teams = {3, 3, 4, 4};

    int matchesSize = 4;
    int **matches = malloc(matchesSize * sizeof(int *));
    for (int i = 0; i < matchesSize; i++)
    {
        matches[i] = malloc(2 * sizeof(int));
        matches[i][0] = win_teams[i];
        matches[i][1] = loss_teams[i];
    }

    for (int i = 0; i < matchesSize; i++)
    {
        for (int j = 0; j < 2; j++)
        {
        }
    }

    return 0;
}