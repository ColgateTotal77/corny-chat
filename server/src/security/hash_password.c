#include <password.h>

unsigned char *hash_password(char *password, char *login) {
    unsigned char *hashed_password = malloc(SHA256_DIGEST_LENGTH); // SHA256_DIGEST_LENGTH = 32
    char* temp = mx_strjoin(password, login); //З'єдную пороль з логіном (логін використовується для додаткового шифрування)

    SHA256((unsigned char*)temp, mx_strlen(temp), hashed_password); //Пороль шифрується в HEX
    free(temp);

    return hashed_password;
}

