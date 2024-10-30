#include "libmx.h"


void *mx_memcpy(void *restrict dst, const void *restrict src, size_t n) {
    unsigned char *restrict byte_dst = (unsigned char *restrict)dst;
    unsigned char *restrict byte_src = (unsigned char *restrict)src;

    for (size_t i = 0; i < n; i++) {
        byte_dst[i] = byte_src[i];
    }

    return dst;
}


