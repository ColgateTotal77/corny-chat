int mx_strlen(const char*);

char* mx_strcat(char* restrict s1, const char* restrict s2) {
	int start = mx_strlen(s1);
	int i = 0;
	while (s2[i]) {
		s1[start] = s2[i];
		start++;
		i++;
	}
	s1[start] = '\0';
	return s1;
}
