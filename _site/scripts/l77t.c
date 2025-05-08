#include <stdio.h>
#include <stdlib.h>

void bubble_sort(int *array, int arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        for (int j = 0; j < (arr_size - i - 1); j++)
        {
            if (array[j] > array[j + 1])
            {
                int temp = array[j + 1];
                array[j + 1] = array[j];
                array[j] = temp;
            }
        }
    }
}

void print_array(int *array, int arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        printf("pos %d: %d\n", i, array[i]);
    }
}

int main()
{
    int arr_size = 20;
    int *arr = malloc(arr_size * sizeof(int));

    for (int i = 0; i < arr_size; i++)
    {
        arr[i] = ((rand() % 10) + 1);
    }

    bubble_sort(arr, arr_size);

    print_array(arr, arr_size);
}
