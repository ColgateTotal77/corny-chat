char* mx_strcpy(char* dst, const char* src) {
	char* temp_dst = dst;

	while (*src != '\0') {
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return temp_dst;
}
