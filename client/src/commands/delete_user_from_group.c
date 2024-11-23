#include "cJSON.h"
#include "commands.h"
#include "client.h"

void delete_user_from_group(SSL *ssl, int user_id, int chat_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", DELETE_USER_FROM_GROUP); 
    cJSON_AddNumberToObject(json, "user_id", user_id);
    cJSON_AddNumberToObject(json, "chat_id", chat_id);

    send_and_delete_json(ssl, &json);
}
