#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// General utils functions
void append_to_intarr(int** arr, int* arr_size, int num);
void append_to_intptrarr(int*** arr, int* arr_size, int* arr_ptr);
void append_to_strarr(char*** arr, int* arr_size, char* str);
bool num_inarray(int* arr, int arr_size, int num);
void cpy_intarr(int** dst, int*src, int size);
void del_intarr(int*** arr, int arr_size);
void del_strarr(char*** arr, int arr_size);


// String functions
int mx_strlen(const char *s);
void mx_strdel(char **str);
void mx_del_strarr(char ***arr);
int mx_get_char_index(const char *str, char c);
char *mx_strcpy(char *dst, const char *src);
char *mx_strncpy(char *dst, const char *src, int len);
int mx_count_words(const char *str, char delimiter);
char *mx_strnew(const int size);
char **mx_strsplit(char const *s, char c);
bool mx_isdigit(int c);
bool mx_isspace(char c);
int mx_atoi(const char *str);
int mx_strncmp(const char *s1, const char *s2, int n);


// Memory management functions
void *mx_memcpy(void *restrict dst, const void *restrict src, size_t n);
void *mx_realloc(void *ptr, size_t size);









