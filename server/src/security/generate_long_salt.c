#include <password.h>

char *generate_long_salt(void) {
    srand(time(NULL));
    char *salt = mx_strnew(32);

    for (int i = 0; i < 31; i++) {
        salt[i] = 'a' + rand() % 26;
    }
    salt[31] = '\0';
    return salt;
}
