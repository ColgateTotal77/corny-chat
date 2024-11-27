#include "time.h"
#include "stdlib.h"
#include "string.h"

char* get_string_time(void) {
    time_t now = time(NULL);
    struct tm *utc_time = gmtime(&now);
    char time_string[20];

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", utc_time);

    char *result = (char*)calloc(20, sizeof(char));
    strcpy(result, time_string);

    return result;
}
