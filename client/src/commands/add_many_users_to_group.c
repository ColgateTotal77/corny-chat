#include "cJSON.h"
#include "commands.h"
#include "client.h"

void add_many_users_to_group(SSL *ssl, int* users_id, int users_count, int chat_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", ADD_MANY_USERS_TO_GROUP);
    cJSON_AddNumberToObject(json, "chat_id", chat_id);
    cJSON *users_id_array = cJSON_CreateIntArray(users_id, users_count);
    cJSON_AddItemToObject(json, "users_id", users_id_array);

    send_and_delete_json(ssl, &json);
}

