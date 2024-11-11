int mx_strlen(const char* s) {
	int counter = 0;

	while (s[counter] != '\0') {
		counter += 1;
	}
	return counter;
}
