#include "cJSON.h"
#include "commands.h"
#include "client.h"

void change_password(SSL *ssl, char *old_password, char *new_password) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", CHANGE_PASSWORD);
    cJSON_AddStringToObject(json, "old_password", old_password);
    cJSON_AddStringToObject(json, "new_password", new_password);

    send_and_delete_json(ssl, &json);
}

