static int hex_to_decimal(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

unsigned long mx_hex_to_nbr(const char* hex) {
	if (!hex || !*hex)
		return 0;
	unsigned long res = 0;

	while (*hex) {
		res = res * 16 + hex_to_decimal(*hex);
		hex++;
	}
	return res;
}
