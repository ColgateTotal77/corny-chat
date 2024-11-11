#include <stddef.h>

void* mx_memchr(const void* s, int c, size_t n);

void* mx_memmem(void* big, size_t big_len, void* little, size_t little_len) {
	if (!big || !big_len || !little || !little_len) {
		return NULL;
	}
	unsigned char* big_ptr = big;
	const unsigned char* little_ptr = little;
	size_t bl = big_len;
	size_t ll = little_len;

	if (little_len > big_len) {
		return NULL;
	}
	if (little_len == 1) {
		return mx_memchr(big_ptr, *little_ptr, bl);
	}
	while ((big_ptr = mx_memchr(big_ptr, *little_ptr, bl))) {
		bl = bl - (big_ptr - (unsigned char*)big);
		unsigned char* start_compare = big_ptr;

		while (ll) {
			if (*little_ptr != *big_ptr) {
				break;
			}
			big_ptr++;
			little_ptr++;
			ll--;
		}
		if (ll == 0)
			return start_compare;
		ll = little_len;
		little_ptr = little;
		big_ptr = start_compare + 1;
	}
	return big_ptr;
}
