#include <stddef.h>
char* mx_strtrim(const char* str);
int mx_strlen(const char* str);
int check_is_whitespase(char c);
void mx_strdel(char** str);
char* mx_strnew(int size);

/**
 *
 * @param str
 * @return new allocated string
 */
char* mx_del_extra_spaces(const char* str) {
	if (str == NULL)
		return NULL;
	char* temp = mx_strtrim(str);

	if (temp == NULL)
		return NULL;
	for (int i = 0; temp[i] != '\0'; i++) {
		if (check_is_whitespase(temp[i]) == 1) {
			if (temp[i] == ' ')
				while (check_is_whitespase(temp[++i]))
					temp[i] = '\f';
			else temp[i] = '\f';
		}
	}
	int j = 0;
	char* res = mx_strnew(mx_strlen(temp));

	if (res == NULL)
		return NULL;
	for (int i = 0; temp[i] != '\0'; i++) {
		if (temp[i] == '\f')
			continue;
		res[j++] = temp[i];
	}
	mx_strdel(&temp);
	return res;
}
