#include "libmx.h"

/**
 *
 * @param arr array of char*
 * @param val char* that it finds index for
 * @return index in array
 */
int mx_get_arr_el_index(char** arr, const char* val) {
	for (int i = 0; arr; i++) {
		if (mx_strcmp(val, arr[i]) == 0)
			return i;
	}
	return -1;
}
