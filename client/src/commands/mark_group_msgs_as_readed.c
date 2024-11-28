#include "cJSON.h"
#include "commands.h"
#include "client.h"

void mark_group_msgs_as_readed(SSL *ssl, int group_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", MARK_GROUP_MSGS_AS_READED);
    cJSON_AddNumberToObject(json, "group_id", group_id);

    send_and_delete_json(ssl, &json);
}
