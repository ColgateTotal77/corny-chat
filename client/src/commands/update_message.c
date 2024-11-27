#include "cJSON.h"
#include "commands.h"
#include "client.h"

void update_message(SSL *ssl, int message_id, char *new_message) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", UPDATE_MESSAGE);
    cJSON_AddNumberToObject(json, "message_id", message_id);
    cJSON_AddStringToObject(json, "new_message", new_message);

    send_and_delete_json(ssl, &json);
}
