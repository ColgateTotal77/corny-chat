int mx_strlen(const char* s);

/*
*  returns the count of sub in str
*  returns 0 if sub is an empty string
 */

int mx_count_substr(const char* str, const char* sub) {
	if (!str || !sub)
		return -1;
	const int needle_len = mx_strlen(sub);
	int counter = 0;

	if (needle_len == 0)
		return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] == sub[0]) {
			for (int j = 0; j < needle_len; j++) {
				if (str[i] != sub[j])
					break;
				if (j == needle_len - 1) {
					counter++;
					continue;
				}
				i += 1;
			}
		}
	}
	return counter;
}
