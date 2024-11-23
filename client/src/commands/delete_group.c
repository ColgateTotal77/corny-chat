#include "cJSON.h"
#include "commands.h"
#include "client.h"

void delete_group(SSL *ssl, int group_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", DELETE_GROUP); 
    cJSON_AddNumberToObject(json, "group_id", group_id);

    send_and_delete_json(ssl, &json);
}
