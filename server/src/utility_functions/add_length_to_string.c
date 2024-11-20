#include "server.h"
#include "../libmx/inc/libmx.h"


char *add_length_to_string(char *str) {
    char *string_length = mx_itoa(strlen(str));
    char *new_str = mx_strjoin(string_length, str);
    
    free(string_length);

    return new_str;
}
