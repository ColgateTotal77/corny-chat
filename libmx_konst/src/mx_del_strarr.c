#include <stdlib.h>

void mx_strdel(char** str);

void mx_del_strarr(char*** arr) {
	if (!arr || !*arr)
		return;
	int i = 0;

	while ((*arr)[i]) {
		mx_strdel(&(*arr)[i]);
		i++;
	}
	free(*arr);
	*arr = NULL;
}
