#include <stddef.h>


void* mx_memchr(const void* s, int c, size_t n) {
	if (!s || !n) {
		return NULL;
	}
	const unsigned char* source = s;
	const unsigned char target = c;

	while (n--) {
		if (*source == target) {
			return (void*)source;
		}
		source++;
	}
	return NULL;
}
