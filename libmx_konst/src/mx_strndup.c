#include <stddef.h>

#include "libmx.h"


char* mx_strndup(const char* s1, size_t n) {
	const size_t size_max = mx_strlen(s1);
	const size_t size = n > size_max ? size_max : n;
	char* newString = mx_strnew((int)size);

	mx_strncpy(newString, s1, size);
	return newString;
}
