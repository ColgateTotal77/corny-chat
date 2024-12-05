#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_chat_users(SSL *ssl, int chat_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_CHAT_USERS); 
    cJSON_AddNumberToObject(json, "chat_id", chat_id);

    send_and_delete_json(ssl, &json);
}
