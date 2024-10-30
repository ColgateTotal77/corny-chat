#include "libmx.h"

void cpy_intarr(int** dst, int*src, int size) {
    for (int i = 0; i < size; i++) {
        (*dst)[i] = src[i];
    }
}


