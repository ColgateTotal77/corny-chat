#include"libmx.h"

int mx_get_substr_index(const char *str, const char *sub) {
    if (str == NULL || sub == NULL) {
        return -2;
    }
    if (*sub == '\0') {
        return 0;
    }

    int len = mx_strlen(sub);
    int str_ind = mx_get_char_index(str, sub[0]);
    if (str_ind < 0) return -1;
    int result_ind = str_ind;
    str = str + str_ind;

    while (str != 0) {
        int comparing = mx_strncmp(str, sub, len);

        if (comparing == 0) {
            return result_ind;
        }

        result_ind += 1;
        str++;

        str_ind = mx_get_char_index(str, sub[0]);
        if (str_ind < 0) return -1;
        result_ind += str_ind;
        str += str_ind;
    }

    return -1;
}



