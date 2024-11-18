#include "time.h"
#include "stdlib.h"
#include "string.h"

char* get_string_time(void) {
    time_t raw_time;
    struct tm *time_info;
    char time_string[20];

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

    char *result = (char*)calloc(20, sizeof(char));
    strcpy(result, time_string);

    return result;
}
