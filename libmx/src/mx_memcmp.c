#include <stddef.h>


int mx_memcmp(const void* s1, const void* s2, size_t n) {
	const unsigned char* bigger = s1;
	const unsigned char* smaller = s2;
	size_t i = 0;

	while (i < n) {
		if (bigger[i] != smaller[i])
			return bigger[i] - smaller[i];
		i++;
	}
	return 0;
}
