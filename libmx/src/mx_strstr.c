#include <stdio.h>

char *mx_strstr(const char *haystack, const char *needle) {
    char *result = NULL; 
    
    if (*needle == '\0') {
        return (char *) haystack;
    }
    for (int i = 0; haystack[i] != '\0'; i++) {
        if (haystack[i] == needle[0]) {
            result = (char *)(haystack + i);
            for(int j = 1; needle[j] != '\0'; j++) {
                if (needle[j] !=  haystack[i + j]) {
                    result = NULL;
                    break;
                }
            }
            if (result != NULL) {
                return result;
            }
        }
    }
    return result;
}
