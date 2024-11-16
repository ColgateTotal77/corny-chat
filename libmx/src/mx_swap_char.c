void mx_swap_char(char* s1, char* s2) {
	if (!s1 || !s2)
		return;
	if (!(*s1 && *s2))
		return;
	const char temp = *s1;
	*s1 = *s2;
	*s2 = temp;
}
