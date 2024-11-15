#include <unistd.h>
char* append_string(char* original, const char* to_append);
int mx_get_char_index(const char* str, char c);
char* mx_strnew(const int size);
void mx_strdel(char** str);
char* mx_strdup(const char* str);
int mx_strlen(const char* s);


int mx_read_line(char** lineptr, size_t buf_size, char delim, const int fd) {
	if (fd == -1)
		return -2;
	char* buffer = mx_strnew((int)buf_size);
	if (buffer == NULL) {
		mx_strdel(&buffer);
		return -2;
	}
	static char* remainder = NULL;
	int ind;
	char* result = mx_strnew(0);

	if (result == NULL) {
		mx_strdel(&buffer);
		mx_strdel(&remainder);
		mx_strdel(&result);
		return -2;
	}
	ssize_t bytes_read = 0;
	int counter = 0;

	if (remainder) {
		ind = mx_get_char_index(remainder, delim);
		if (ind > -1) {
			remainder[ind] = '\0';
			counter = mx_strlen(remainder);
			result = append_string(result, remainder);
			*lineptr = result;
			char* new_remainder = mx_strdup(remainder + ind + 1);
			if (new_remainder == NULL) {
				mx_strdel(&buffer);
				mx_strdel(&remainder);
				mx_strdel(&result);
				mx_strdel(&new_remainder);
				return -2;
			}
			mx_strdel(&remainder);
			remainder = mx_strdup(new_remainder);
			mx_strdel(&new_remainder);
			return counter;
		}
		counter = mx_strlen(remainder);
		result = append_string(result, remainder);
		if (!result) {
			mx_strdel(&buffer);
			mx_strdel(&remainder);
			mx_strdel(&result);
			return -2;
		}
	}
	while ((bytes_read = read(fd, buffer, buf_size)) > 0) {
		if (bytes_read == -1) {
			mx_strdel(&buffer);
			mx_strdel(&remainder);
			mx_strdel(&result);
			return -2;
		}
		if (bytes_read == 0) {
			mx_strdel(&buffer);
			mx_strdel(&remainder);
			mx_strdel(&result);
			break;
		}
		counter += (int)bytes_read;
		buffer[bytes_read] = '\0';
		ind = mx_get_char_index(buffer, delim);
		if (ind > -1) {
			buffer[ind] = '\0';
			counter = counter - (int)bytes_read + ind;
			result = append_string(result, buffer);
			if (!result) {
				mx_strdel(&buffer);
				mx_strdel(&remainder);
				mx_strdel(&result);
				return -2;
			}
			mx_strdel(&remainder);
			remainder = mx_strdup(buffer + ind + 1);
			break;
		}

		result = append_string(result, buffer);
		if (!result) {
			mx_strdel(&buffer);
			mx_strdel(&remainder);
			mx_strdel(&result);
			return -2;
		}
	}
	mx_strdel(&buffer);
	*lineptr = result;
	return counter;
}
