#include "libmx.h"

void append_to_intarr(int** arr, int* arr_size, int num) {
    *arr = (int*)mx_realloc(*arr, (*arr_size + 1) * sizeof(int));
    (*arr)[*arr_size] = num;
    *arr_size = *arr_size + 1;
}


