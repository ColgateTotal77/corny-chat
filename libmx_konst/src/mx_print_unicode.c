#include <unistd.h>
#include <wchar.h>

void mx_printchar(char c);

static int get_wchar_len(const wchar_t wc) {
	if (wc < 0x80)
		return 1;
	if (wc < 0x7FF)
		return 2;
	if (wc < 0xFFFF)
		return 3;
	if (wc < 0x10FFFF)
		return 4;
	return 0;
}

static char* code_2_byte(const wchar_t w2, char* utf) {
	utf[0] = (w2 >> 6) | 0xC0;
	utf[1] = (w2 & 0x3F) | 0x80;
	return utf;
}

static char* code_3_byte(const wchar_t w3, char* utf) {
	utf[0] = (w3 >> 12) | 0xE0;
	utf[1] = ((w3 >> 6) & 0x3F) | 0x80;
	utf[2] = (w3 & 0x3F) | 0x80;
	return utf;
}

static char* code_4_byte(const wchar_t w4, char* utf) {
	utf[0] = (w4 >> 18) | 0xF0;
	utf[1] = ((w4 >> 12) & 0x3F) | 0x80;
	utf[2] = ((w4 >> 6) & 0x3F) | 0x80;
	utf[3] = (w4 & 0x3F) | 0x80;
	return utf;
}

void mx_print_unicode(wchar_t c) {
	const int len = get_wchar_len(c);
	char wc[len];

	switch (len) {
	case 1: mx_printchar(c);
		break;
	case 2: write(1, code_2_byte(c, wc), 2);
		break;
	case 3: write(1, code_3_byte(c, wc), 3);
		break;
	case 4: write(1, code_4_byte(c, wc), 4);
		break;
	default: ;
	}
}
