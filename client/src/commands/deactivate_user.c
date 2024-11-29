#include "cJSON.h"
#include "commands.h"
#include "client.h"

void deactivate_user(SSL *ssl, char *user_login) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", DEACTIVATE_USER); 
    cJSON_AddStringToObject(json, "login", user_login);

    send_and_delete_json(ssl, &json);
}
