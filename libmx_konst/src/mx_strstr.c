#include <stddef.h>

int mx_strlen(const char* s);

/*
 * returns the pointer to the fist symbol of needle in haystack
 */

char* mx_strstr(const char* haystack, const char* needle) {
	if (!haystack || !needle || mx_strlen(needle) > mx_strlen(haystack))
		return NULL;
	if (needle[0] == '\0')
		return (char*)haystack;
	const int needle_len = mx_strlen(needle);

	for (int i = 0; haystack[i] != '\0'; i++) {
		if (haystack[i] == needle[0]) {
			for (int j = 1, k = i + 1; j < needle_len; j++, k++) {
				if (haystack[k] != needle[j])
					break;
				if (j == needle_len - 1)
					return (char*)&haystack[i];
			}
		}
	}
	return NULL;
}
