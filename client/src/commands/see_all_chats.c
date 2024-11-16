#include "cJSON.h"
#include "commands.h"
#include "client.h"

void see_all_chats(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEE_ALL_CHATS);

    send_and_delete_json(ssl, &json);
}


