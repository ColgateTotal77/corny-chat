#include <stddef.h>
void* mx_memcpy(void* restrict dst, const void* restrict src, size_t n);

void* mx_memmove(void* dst, void* src, size_t len) {
	if (!dst || !src) {
		return NULL;
	}
	if (dst == src) {
		return dst;
	}
	if (dst < src) {
		return mx_memcpy(dst, src, len);
	}
	unsigned char* recipient = dst;
	const unsigned char* source = src;

	recipient = recipient + len - 1;
	source = source + len - 1;
	while (len--) {
		*recipient-- = *source--;
	}
	return dst;
}
