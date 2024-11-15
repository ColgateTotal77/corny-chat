#include <stddef.h>

int mx_count_substr(const char* str, const char* sub);
int mx_get_substr_index(const char* str, const char* sub);
char* mx_strnew(const int size);
int mx_strlen(const char* s);
char* mx_strdup(const char* str);
char* mx_strncpy(char* dst, const char* src, size_t n);


char* mx_replace_substr(const char* str, const char* sub, const char* replace) {
	if (!str || !sub || !replace) {
		return NULL;
	}
	const int counter = mx_count_substr(str, sub);

	if (sub[0] == '\0' || counter == 0)
		return mx_strdup(str);
	int stop_point = 0;

	char* result = mx_strnew(mx_strlen(str) + counter * (mx_strlen(replace) - mx_strlen(sub)) + 1);
	for (int i = 0; i < counter; i++) {
		const int needle_ind = mx_get_substr_index(str, sub);
		mx_strncpy(&result[stop_point], str, needle_ind);
		mx_strncpy(&result[stop_point + needle_ind], replace, mx_strlen(replace));
		str = str + needle_ind + mx_strlen(sub);
		stop_point = stop_point + needle_ind + mx_strlen(replace);
	}
	return result;
}
