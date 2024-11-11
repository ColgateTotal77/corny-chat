int mx_strlen(const char* s);

int mx_get_substr_index(const char* str, const char* sub) {
	if (!str || !sub)
		return -2;
	const int needle_len = mx_strlen(sub);

	if (needle_len == 0)
		return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] == sub[0]) {
			for (int j = 0, k = i; j < needle_len; j++, k++) {
				if (str[k] != sub[j])
					break;
				if (j == needle_len - 1)
					return i;
			}
		}
	}
	return -1;
}
