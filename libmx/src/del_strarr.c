#include "libmx.h"

void del_strarr(char*** arr, int arr_size) {
    for (int i = 0; i < arr_size; i++) {
        free((*arr)[i]);
    }
    free(*arr);
}


