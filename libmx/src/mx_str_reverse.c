int mx_strlen(const char* s);
void mx_swap_char(char* s1, char* s2);


void mx_str_reverse(char* s) {
	if (!s)
		return;
	int l = 0;
	int r = mx_strlen(s) - 1;
	while (l < r) {
		mx_swap_char(&s[l], &s[r]);
		l++;
		r--;
	}
}
