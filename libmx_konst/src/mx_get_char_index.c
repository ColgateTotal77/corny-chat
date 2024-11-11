/*
*  returns thr index of the first occurrence
* returns -1 if no occurrence is found
* returns -2 if the string does not exist
 */


int mx_get_char_index(const char* str, char c) {
	if (!str || !c)
		return -2;
	int marker = 0;

	while (str[marker]) {
		if (str[marker] == c)
			return marker;
		marker++;
	}
	return -1;
}
