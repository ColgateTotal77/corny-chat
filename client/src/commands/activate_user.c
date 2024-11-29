#include "cJSON.h"
#include "commands.h"
#include "client.h"

void activate_user(SSL *ssl, char *login) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", ACTIVATE_USER); 
    cJSON_AddStringToObject(json, "user_login", login);

    send_and_delete_json(ssl, &json);
}
