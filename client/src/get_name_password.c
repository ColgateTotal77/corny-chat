#include "../inc/client.h"
#include "../inc/password.h"

char* get_name(void) {
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

char* get_password(void) {
    printf("Password must be less than 20 and more than 8 characters. Also, only keyboard symbols are allowed, without spaces.\n");
    printf("Enter your password: ");
    char password[22];
    fgets(password, 22, stdin);
    str_del_newline(password, strlen(password));
    int password_length = strlen(password);

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
