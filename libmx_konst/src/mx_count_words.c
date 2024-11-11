static void switcher(int* condition, int* count, int trigger) {
	if (*condition == 0 && trigger == 1)
		*condition = 1;
	else if (*condition == 1 && trigger == 0) {
		*condition = 0;
		*count += 1;
	}
}

int mx_count_words(const char* str, char c) {
	if (!str || !c)
		return -1;
	int condition = 1;
	int count = 0;

	for (int i = 0; str[i] != '\0'; i++) {
		switcher(&condition, &count, str[i] == c);
	}
	return count;
}
