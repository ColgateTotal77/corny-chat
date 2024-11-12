#include "client.h"
#include "password.h"

bool check_password(char *password) { //Валідація паролю
    if (password == NULL) { //Перевірка на порожній рядок
        return false;
    }

    int password_length = mx_strlen(password);

    if (password_length < 8 || password_length > 20) { //Перевірка на довжину, яка має бути більше 7 і менше 20
        return false;
    }

    for (int i = 0; i < password_length; i++) {
        if (mx_check_space(password[i]) || password[i] < 33 || password[i] > 126) { //Перевірка пароля на невалідні символи по ASCII, тут дозволені від '!' до '~'
            return false;
        }
    }
    return true;
}

static char* get_name(void) {
    printf("Name must be less than 30 and more than 1 characters.\n");
    printf("Please enter your name: ");
    char name[32];
    fgets(name, 32, stdin);
    str_del_newline(name, strlen(name));
    int name_length = strlen(name);

	if (name_length > 30 || name_length < 2) {
		printf("Invalid input. Name must be less than 30 and more than 1 characters.\n");
		exit(EXIT_FAILURE);
	}

    char *name_buffer = (char*)malloc(name_length * sizeof(char) + 1);
    name_buffer[name_length] = '\0';
    strncpy(name_buffer, name, name_length);

    return name_buffer;
}

static char* get_password(void) {
    printf("Password must be less than 20 and more than 8 characters. Also, only keyboard symbols are allowed, without spaces.\n");
    printf("Enter your password: ");
    char password[22];
    fgets(password, 22, stdin);
    str_del_newline(password, strlen(password));
    int password_length = strlen(password);
    if (!check_password(password)) {
		printf("Invalid input. Password is incorrest!.\n");
		exit(EXIT_FAILURE);
	}
    printf("%d", password_length);
    char *password_buffer = (char*)malloc(password_length * sizeof(char) + 1);
    password_buffer[password_length] = '\0';
    strncpy(password_buffer, password, password_length);

    return password_buffer;
}

cJSON* get_name_password(void) {
    char *name = get_name();
    char *password = get_password();

    cJSON *json = cJSON_CreateObject(); 
    cJSON_AddStringToObject(json, "name", name); 
    cJSON_AddStringToObject(json, "password", password);

    free(name);
    free(password);

    return json;
}

