int mx_check_space (const char c) {
    return c == ' ' || c == '\t' || c == '\n' ||
            c == '\f' || c == '\r' || c == '\v';
}
