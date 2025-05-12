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

int **findWinners(int **matches, int matchesSize, int *matchesColSize, int *returnSize, int **returnColumnSizes)
{
    *matchesColSize = 2;
    *returnSize = 2;

    // Start by finding out how big the hash table should be
    // Team numbers start at 1
    int max_team_number = 0;
    for (int i = 0; i < matchesSize; i++)
    {
        int match_max_team = max(matches[i][0], matches[i][1]);
        max_team_number = max(match_max_team, max_team_number);
    }

    // Hash function can just be team_number - 1, perfect to preserve order
    // Also track the zero and single loss counts
    int *hash_map = (int *)calloc(max_team_number, sizeof(int));

    // Initialize at -1
    for (int i = 0; i < max_team_number; i++)
    {
        hash_map[i] = -1;
    }

    int win_team, loss_team;
    int cnt_zeros = 0;
    int cnt_singles = 0;

    // I stop counting from 2 losses basically
    for (int i = 0; i < matchesSize; i++)
    {
        win_team = matches[i][0];
        loss_team = matches[i][1];

        if (hash_map[win_team - 1] == -1)
        {
            hash_map[win_team - 1] = 0;
            cnt_zeros++;
        }

        if (hash_map[loss_team - 1] == -1)
        {
            hash_map[loss_team - 1] = 1;
            cnt_singles++;
        }
        else if (hash_map[loss_team - 1] == 0)
        {
            hash_map[loss_team - 1]++;
            cnt_singles++;
            cnt_zeros--;
        }
        else if (hash_map[loss_team - 1] == 1)
        {
            hash_map[loss_team - 1]++;
            cnt_singles--;
        }
    }

    // Fill in the zero and single loss arrays
    int *zeros_arr = malloc(cnt_zeros * sizeof(int));
    int *singles_arr = malloc(cnt_singles * sizeof(int));

    int pos_zeros = 0;
    int pos_singles = 0;

    for (int i = 0; i < max_team_number; i++)
    {
        if (hash_map[i] == 0)
        {
            // +1 to retrieve the team number
            zeros_arr[pos_zeros] = i + 1;
            pos_zeros++;
        }
        else if (hash_map[i] == 1)
        {
            singles_arr[pos_singles] = i + 1;
            pos_singles++;
        }
    }

    int **return_array = malloc((*returnSize) * sizeof(int *));

    *returnColumnSizes = malloc(sizeof(*returnSize) * sizeof(int));
    (*returnColumnSizes)[0] = cnt_zeros;
    (*returnColumnSizes)[1] = cnt_singles;

    return_array[0] = zeros_arr;
    return_array[1] = singles_arr;

    free(hash_map);

    return return_array;
}

int main()
{
    int win_teams[] = {2, 1, 5, 6};
    int loss_teams[] = {3, 3, 4, 4};

    int matchesSize = sizeof(win_teams) / sizeof(win_teams[0]);
    int **matches = malloc(matchesSize * sizeof(int *));
    for (int i = 0; i < matchesSize; i++)
    {
        matches[i] = malloc(2 * sizeof(int));
        matches[i][0] = win_teams[i];
        matches[i][1] = loss_teams[i];
    }

    int *matchesColSize = malloc(sizeof(int));
    int *returnSize = malloc(sizeof(int));
    int **returnColumnSizes = malloc(2 * sizeof(int *));
    for (int i = 0; i < 2; i++)
    {
        returnColumnSizes[i] = malloc(sizeof(int));
    }

    int **return_array = findWinners(matches, matchesSize, matchesColSize, returnSize, returnColumnSizes);

    // Print the zeros and then the ones arrays
    printf("[");
    for (int i = 0; i < *returnColumnSizes[0]; i++)
    {
        if (i == (*returnColumnSizes[0] - 1))
        {
            printf("%d", return_array[0][i]);
        }
        else
        {
            printf("%d, ", return_array[0][i]);
        }
    }
    printf("]\n");

    // Print the zeros and then the ones arrays
    printf("\n[");
    for (int i = 0; i < *returnColumnSizes[1]; i++)
    {
        if (i == (*returnColumnSizes[1] - 1))
        {
            printf("%d", return_array[1][i]);
        }
        else
        {
            printf("%d, ", return_array[1][i]);
        }
    }
    printf("]\n");

    for (int i = 0; i < matchesSize; i++)
    {
        free(matches[i]);
    }
    free(matches);
    free(matchesColSize);
    free(returnSize);
    free(returnColumnSizes);
    free(return_array[0]); // Free the zeros array
    free(return_array[1]); // Free the singles array
    free(return_array);

    return 0;
}
