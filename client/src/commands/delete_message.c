#include "cJSON.h"
#include "commands.h"
#include "client.h"

void delete_message(SSL *ssl, int message_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", DELETE_MESSAGE);
    cJSON_AddNumberToObject(json, "message_id", message_id);

    send_and_delete_json(ssl, &json);
}
