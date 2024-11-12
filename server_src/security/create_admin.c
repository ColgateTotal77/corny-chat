#include <password.h>
#include <stdio.h>

unsigned char *create_admin(void) {
    char *password = generate_password();
    FILE *file = fopen("admin.txt", "w");
    fprintf(file, "%s", password);
    fclose(file);

    printf("Ваш логін: \"FirstAdmin\"\nВаш пароль: \"%s\"\nТак само він знаходиться в директорії месенджера у файлі admin.txt", password);
    return hash_password(password, "FirstAdmin");
}

