#include "libmx.h"

void del_intarr(int*** arr, int arr_size) {
    for (int i = 0; i < arr_size; i++) {
        free((*arr)[i]);
    }
    free(*arr);
}


