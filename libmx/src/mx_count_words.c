int mx_count_words(const char *str, char delimiter) {
    int count;

    if (*str == delimiter) {
        count = 0;
        str++;
    }
    else if (*str == '\0') {
        return 0;
    }
    else {
        count = 1;
        str++;
    }

    while (*str != '\0') {
        if (*(str - 1) == delimiter && *str != delimiter) {
            count++;
        }

        str++;
    }

    return count;
}

