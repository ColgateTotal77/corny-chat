#include "cJSON.h"
#include "commands.h"
#include "client.h"

void create_chat(SSL *ssl, char *new_chat_name) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", CREATE_CHAT);
    cJSON_AddStringToObject(json, "new_chat_name", new_chat_name);

    send_and_delete_json(ssl, &json);
}

