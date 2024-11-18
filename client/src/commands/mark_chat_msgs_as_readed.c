#include "cJSON.h"
#include "commands.h"
#include "client.h"

void mark_chat_msgs_as_readed(SSL *ssl, int sender_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", MARK_CHAT_MSGS_AS_READED);
    cJSON_AddNumberToObject(json, "sender_id", sender_id);

    send_and_delete_json(ssl, &json);
}
