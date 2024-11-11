int mx_strcmp(const char* s1, const char* s2);

int mx_binary_search(char** arr, int size, const char* s, int* count) {
	int left = 0;
	int right = size - 1;
	while (left <= right) {
		*count += 1;
		const int i = left + (right - left) / 2;
		if (mx_strcmp(arr[i], s) == 0)
			return i;
		if (mx_strcmp(arr[i], s) < 0) {
			left = i + 1;
			continue;
		}
		if (mx_strcmp(arr[i], s) > 0)
			right = i - 1;
	}
	*count = 0;
	return -1;
}
