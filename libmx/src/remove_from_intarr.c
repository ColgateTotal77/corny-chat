#include "libmx.h"

void remove_from_intarr(int** arr, int* arr_size, int num) {
    int new_size = *arr_size - 1;
    if (new_size < 0) {
        return;
    }

    if (new_size == 0) {
        if ((*arr)[0] == num) {
            free(*arr);
            *arr = NULL;
            *arr_size = 0;
        }
        return;
    }

    int *new_arr = (int*)malloc((new_size) * sizeof(int));
    
    int i = 0;
    int j = 0;
    while (i < *arr_size) {
        if ((*arr)[i] == num) {
            i++;
            continue;
        }

        new_arr[j] = (*arr)[i];
        j++;
        i++;
    }

    free(*arr);
    *arr = new_arr;
    *arr_size = new_size;
}

