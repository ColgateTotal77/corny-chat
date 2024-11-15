#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
int mx_strlen(const char* s);
char* mx_strcat(char* restrict s1, const char* restrict s2);
char* mx_strnew(const int size);
void mx_strdel(char** str);
void* mx_realloc(void* ptr, size_t size);

/*
 * 133179 symbols - no exceptions but missed a part
 * 133144 is Ok
 */
char* append_string(char* original, const char* to_append) {
	if (to_append == NULL) {
		return original;
	}
	const size_t original_len = original ? mx_strlen(original) : 0;
	const size_t append_len = mx_strlen(to_append);

	char* new_original = mx_realloc(original, original_len + append_len + 1);
	if (!new_original)
		return NULL;

	mx_strcat(new_original, to_append);
	return new_original;
}


char* mx_file_to_str(const char* file) {
	const int file_content = open(file, O_RDONLY);
	if (file_content == -1) {
		return NULL;
	}
	const int buffer_size = 1024;
	char* buffer = mx_strnew(buffer_size);
	if (buffer == NULL) {
		close(file_content);
		return NULL;
	}
	ssize_t bytes_read = 0;
	char* result = mx_strnew(0);

	while ((bytes_read = read(file_content, buffer, buffer_size)) > 0) {
		if (bytes_read == -1) {
			mx_strdel(&result);
			mx_strdel(&buffer);
			close(file_content);
			return NULL;
		}
		buffer[bytes_read] = '\0';
		result = append_string(result, buffer);
		if (result == NULL) {
			mx_strdel(&buffer);
			mx_strdel(&result);
			close(file_content);
			return NULL;
		}
	}
	mx_strdel(&buffer);
	close(file_content);
	return result;
}
