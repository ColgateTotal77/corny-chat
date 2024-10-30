#include "libmx.h"

void append_to_intptrarr(int*** arr, int* arr_size, int* arr_ptr) {
    *arr = (int**)mx_realloc(*arr, (*arr_size + 1) * sizeof(int*));
    (*arr)[*arr_size] = arr_ptr;
    *arr_size = *arr_size + 1;
}


