#include <password.h>

char *generate_salt(void) {
    srand(time(NULL));
    char *salt = mx_strnew(16);

    for (int i = 0; i < 15; i++) {
        salt[i] = 'a' + rand() % 26;
    }
    salt[15] = '\0';
    return salt;
}
