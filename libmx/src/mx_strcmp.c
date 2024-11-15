int mx_strcmp(const char* s1, const char* s2) {
	int l = 0;

	while (s1[l] == s2[l] && s1[l] != '\0' && s2[l] != '\0')
		l++;
	return s1[l] - s2[l];
}
