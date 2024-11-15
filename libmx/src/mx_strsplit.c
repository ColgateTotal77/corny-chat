#include <stdlib.h>

int mx_count_words(const char* str, char c);
void mx_printint(int n);
char* mx_strndup(const char* s1, size_t n);
int mx_strlen(const char* str);
void mx_del_strarr(char*** arr);


/**
 * @brief Splits the string by splitter c and returns an array of string.
 *
 * The function returns a dynamically allocated array.
 * The caller must free the allocated memory after use by mx_del_strarr.
 *
 * @param s The string to split.
 * @param c The char - splitter.
 * @return A pointer to the array of string or NULL if allocation fails or wrong param.
 */

char** mx_strsplit(const char* s, char c) {
	if (!s)
		return NULL;
	const int len = mx_count_words(s, c);
	char** result = malloc(sizeof(char*) * (len + 1));
	if (!result)
		return NULL;
	for (int i = 0; i <= len; i++) {
		result[i] = NULL;
	}
	int counter = 0;
	const int s_len = mx_strlen(s);
	for (int i = 0; i < s_len; i++) {
		if (s[i] != c) {
			const int start = i;
			while (s[i] != c && s[i] != '\0') {
				i++;
			}
			const int end = i;
			result[counter] = mx_strndup(&s[start], end - start);
			if (!result[counter]) {
				mx_del_strarr(&result);
				return NULL;
			}
			counter++;
		}
	}
	result[counter] = NULL;
	return result;
}
