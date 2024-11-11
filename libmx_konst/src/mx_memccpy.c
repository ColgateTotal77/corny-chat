#include <stddef.h>


void* mx_memccpy(void* restrict dst, const void* restrict src, int c, size_t n) {
	unsigned char* recipient = dst;
	const unsigned char* source = src;

	while (n--) {
		if (*source == c) {
			*recipient++ = *source; //save the last one char
			return recipient; // because of ++ above returns next after last saved
		}
		*recipient++ = *source++;
	}
	return NULL;
}
