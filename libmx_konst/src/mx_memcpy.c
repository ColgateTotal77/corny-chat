#include <stddef.h>

void* mx_memcpy(void* restrict dst, const void* restrict src, size_t n) {
	if (!dst || !src) {
		return NULL;
	}
	if (n <= 0) {
		return dst;
	}
	unsigned char* recipient = dst;
	const unsigned char* source = src;

	while (n--) {
		*recipient++ = *source++;
	}
	return dst;
}
