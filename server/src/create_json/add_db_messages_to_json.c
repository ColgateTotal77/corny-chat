#include "server.h"
#include "sql.h"


static cJSON *init_message_json(s_message db_message) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "msg_id", db_message.id);
    cJSON_AddStringToObject(message_json, "created_at", db_message.createdAt);
    cJSON_AddNumberToObject(message_json, "owner_id", db_message.owner_id);
    cJSON_AddNumberToObject(message_json, "target_id", db_message.target_id);
    cJSON_AddNumberToObject(message_json, "target_group_id", db_message.target_group_id);
    cJSON_AddStringToObject(message_json, "message", db_message.message);
    cJSON_AddBoolToObject(message_json, "readed", db_message.readed);

    return message_json;
}

void add_db_messages_to_json(s_message* db_messages, int messages_num, cJSON **json, char *array_name) {
    cJSON *messages_json = cJSON_AddArrayToObject(*json, array_name);

    for (int i = 0; i < messages_num; i++) {
        s_message new_message = db_messages[i];

        cJSON *new_message_json = init_message_json(new_message);

        cJSON_AddItemToArray(messages_json, new_message_json);
    }

}
