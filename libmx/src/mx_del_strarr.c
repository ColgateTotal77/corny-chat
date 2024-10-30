#include <stdlib.h>
#include"libmx.h"


void mx_del_strarr(char ***arr) {
    if (arr == NULL || *arr == NULL) {
        return;
    }

    char **temp = *arr;

    int i;
    for (i = 0; *(temp + i); i++) {
        free(*(temp + i));
    }
    free(temp);
}


