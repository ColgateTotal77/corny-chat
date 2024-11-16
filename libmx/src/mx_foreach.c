void mx_foreach(int* arr, int size, void (*f)(int)) {
	if (!arr || !size || !f || size <= 0)
		return;

	for (int i = 0; i < size; i++)
		f(arr[i]);
}
