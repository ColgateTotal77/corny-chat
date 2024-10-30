#include "libmx.h"

void append_to_strarr(char*** arr, int* arr_size, char* str) {
    *arr = (char**)mx_realloc(*arr, (*arr_size + 1) * sizeof(char*));
    (*arr)[*arr_size] = str;
    *arr_size = *arr_size + 1;
}


