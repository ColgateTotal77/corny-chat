#include <libmx.h>

char *mx_strjoin(const char *s1, const char *s2) {
    
    if (s1 == NULL && s2 == NULL) {
        return NULL;
    }

    int length1 = 0;
    int length2 = 0;

    if (s1 != NULL) {
        length1 = mx_strlen(s1);
    }

    if (s2 != NULL) {
        length2 = mx_strlen(s2);
    }

    char *result = mx_strnew(length1 + length2); 
    int i;
    
    if (s1 != NULL) {
        for (i = 0; s1[i] != '\0' ;i++) {
            result[i] = s1[i];
        }
    }
    if (s2 != NULL) {
        for (int j = 0; s2[j] != '\0' ;j++) {    
            result[i] = s2[j];
            i++;
        }
    }
    return result;
}
