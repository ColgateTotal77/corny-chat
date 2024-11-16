#include <password.h>
#include <stdio.h>

unsigned char *create_admin(void) {
    char *password = generate_password();
    FILE *file = fopen("admin.txt", "w");
    fprintf(file, "%s", password);
    fclose(file);

    printf("Ваш логін: \"FirstAdmin\"\n"
           "Ваш пароль: \"%s\"\n"
           "Так само він знаходиться в директорії месенджера у файлі admin.txt\n", password);
    unsigned char* new_password_hash = hash_password(password, "FirstAdmin");
    free(password);
    return new_password_hash;
}

