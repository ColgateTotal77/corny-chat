void mx_printchar(char c);

void mx_printint(int n) {
	unsigned int m;

	if (n < 0) {
		mx_printchar('-');
		m = -n;
	}
	else
		m = n;
	if (m != 0 && m / 10 != 0)
		mx_printint(m / 10);
	mx_printchar(m % 10 + '0');
}
