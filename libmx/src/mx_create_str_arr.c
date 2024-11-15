#include <stdlib.h>

#include "libmx.h"

/**
 * create array of string with length = len
 * initiate each element with NULL and determinate with NULL
 * @param len
 * @return pointer to array
 */
char** mx_create_str_arr(const int len) {
	char** list = malloc((len +1) * sizeof(char*));
	for (int i = 0; i <= len; i++) {
		list[i] = NULL;
	}
	// list[len]= NULL;
	return list;
}
