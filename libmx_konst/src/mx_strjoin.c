#include <stddef.h>

char* mx_strnew(int size);
int mx_strlen(const char* str);
char* mx_strcpy(char* dst, const char* src);
char* mx_strncpy(char* dst, const char* src, size_t n);
char* mx_strdup(const char* str);

/**
 * @brief Joins the strings in order s1 + s2  c and returns a new allocated string.
 *
 * The function returns a dynamically allocated string.
 * The caller must free the allocated memory after use by mx_del_strarr.
 * If any of params is NULL returns a new copy TRUE s
 *
 * @param s1 The string staying at firs place.
 * @param s2 TThe string staying at firs place.
 * @return A pointer to the new allocated string or NULL if allocation fails or both params are FALSE.
 */

char* mx_strjoin(const char* s1, const char* s2) {
	if (!s1 && !s2)
		return NULL;
	if (!s1) {
		return mx_strdup(s2);
	}
	if (!s2)
		return mx_strdup(s1);
	const int len1 = mx_strlen(s1);
	char* new_str = mx_strnew(len1 + mx_strlen(s2) + 1);

	mx_strcpy(new_str, s1);
	mx_strncpy(new_str + len1, s2, mx_strlen(s2));
	return new_str;
}
