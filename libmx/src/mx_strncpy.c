#include <stddef.h>

char* mx_strncpy(char* dst, const char* src, size_t n) {
	char* temp_dst = dst;
	size_t counter = 0;
	while (counter < n && *src != '\0') {
		*dst = *src;
		dst++;
		src++;
		counter++;
	}
	// *dst = '\0';
	return temp_dst;
}
