#include "stddef.h"
char* mx_strcpy(char* dst, const char* src);
int mx_strlen(const char* s);
char* mx_strnew(int size);

/*
 * return a new string equals to str
 */


char* mx_strdup(const char* str) {
	if (!str)
		return NULL;
	const int size = mx_strlen(str);
	char* newString = mx_strnew(size);
	if (!newString)
		return NULL;
	mx_strcpy(newString, str);
	return newString;
}
