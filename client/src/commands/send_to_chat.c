#include "cJSON.h"
#include "commands.h"
#include "client.h"

void send_to_chat(SSL *ssl, int chat_id, char *message) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEND_TO_CHAT); 
    cJSON_AddNumberToObject(json, "chat_id", chat_id);
    cJSON_AddStringToObject(json, "message", message);

    send_and_delete_json(ssl, &json);
}


