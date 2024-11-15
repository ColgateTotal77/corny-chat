#include <stddef.h>
void mx_printstr(const char* s);
int mx_strlen(const char* s);

int mx_get_arr_len(char** arr) {
	int len = 0;

	while (arr[len])
		len++;
	return len;
}

/**
 *
 * @param arr
 * @param delim  char after each element
 */
void mx_print_strarr(char** arr, const char* delim) {
	const int len = mx_get_arr_len(arr);

	if (len == 0 || mx_strlen(delim) == 0)
		return;
	for (int i = 0; i < len; i++) {
		if (i == 0)
			mx_printstr(arr[i]);
		else {
			mx_printstr(delim);
			mx_printstr(arr[i]);
		}
	}mx_printstr("\n");
}
