#include "libmx.h"


int mx_get_char_index(const char *str, char c) {
    if (str == NULL) {
        return -2;
    }
    int index = 0;

    while(*str != '\0') {
        if (*str == c) {
            return index;
        }

        index++;
        str++;
    }

    return -1;
}



