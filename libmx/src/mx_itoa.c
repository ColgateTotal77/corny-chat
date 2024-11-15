#include <limits.h>
#include <stddef.h>
char* mx_strnew(const int size);
void mx_str_reverse(char* s);


char* mx_itoa(int number) {
	char* str = mx_strnew(11);

	if (number == 0) {
		str[0] = '0';
		return str;
	}
	char flag = '\0';
	int cofic = 1;
	if (number < 0) {
		flag = '-';
		cofic *= -1;
	}
	int i = 0;
	while (number != 0) {
		str[i++] = (number % 10) * cofic + '0';
		number = number / 10;
	}
	str[i] = flag;
	mx_str_reverse(str);
	return str;
}
