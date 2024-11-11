int mx_strlen(const char* s);

int mx_quicksort(char** arr, int left, int right) {
	if (left >= right)
		return 0;
	int l = left;
	int r = right;
	int counter = 0;
	const int midl = mx_strlen(arr[left + (right - left) / 2]);

	while (l < r) {
		while (mx_strlen(arr[l]) < midl)
			l++;
		while (mx_strlen(arr[r]) > midl)
			r--;
		if (l <= r) {
			if (mx_strlen(arr[l]) != mx_strlen(arr[r])) {
				char* temp = arr[l];
				arr[l] = arr[r];
				arr[r] = temp;
				counter++;
			}
			l++;
			r--;
		}
	}
	if (left < r)
		counter += mx_quicksort(arr, left, r);
	if (l < right)
		counter += mx_quicksort(arr, l, right);
	return counter;
}
