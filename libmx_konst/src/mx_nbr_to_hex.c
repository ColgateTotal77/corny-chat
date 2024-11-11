#include <stddef.h>

char* mx_strnew(const int size);

const char* hex_chars = "0123456789abcdef";

static int get_len(unsigned long n) {
	int counter = 0;
	while ((n = n / 16) > 0) {
		counter++;
	}
	return counter;
}

/**
 * @brief Converts an unsigned integer to a hexadecimal string.
 *
 * The function returns a dynamically allocated string that represents
 * the integer in hexadecimal format. The caller must free the allocated
 * memory after use.
 *
 * @param nbr The unsigned integer to convert.
 * @return A pointer to the hexadecimal string, or NULL if allocation fails.
 */

char* mx_nbr_to_hex(unsigned long nbr) {
	int counter = get_len(nbr);
	char* res = mx_strnew(counter + 2);

	if (!res)
		return NULL;
	while (nbr / 16 > 0) {
		res[counter] = hex_chars[nbr % 16];
		nbr = nbr / 16;
		counter--;
	}
	res[counter] = hex_chars[nbr];
	return res;
}
