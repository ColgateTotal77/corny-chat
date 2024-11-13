#include "cJSON.h"
#include "commands.h"
#include "client.h"

void send_to_user(SSL *ssl, int reciever_id, char *message) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEND_TO_USER);
    cJSON_AddNumberToObject(json, "reciever_id", reciever_id);
    cJSON_AddStringToObject(json, "message", message);

    send_and_delete_json(ssl, &json);
}

