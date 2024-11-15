#include <stddef.h>

#include "libmx.h"

int mx_strlen(const char* s);
char* mx_strndup(const char* s1, size_t n);

int check_is_whitespase(char c) {
	switch (c) {
	case ' ':
	case '\n':
	case '\t':
	case '\v':
	case '\r':
	case '\f':
	case '\'':
	case '\"': return 1;
	default: return 0;
	}
}

char* mx_strtrim(const char* str) {
	if (!str)
		return NULL;
	const int len = mx_strlen(str);
	if (len == 0)
		return mx_strnew(0);
	int left = 0;
	int right = len - 1;

	while (check_is_whitespase(str[left]))
		left++;
	while (check_is_whitespase(str[right]))
		right--;
	char* res = mx_strndup(str + left, right - left + 1);
	if (res == NULL)
		return NULL;
	return res;
}
