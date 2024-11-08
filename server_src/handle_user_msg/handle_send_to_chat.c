#include "server.h"
#include "cJSON.h"

void handle_send_to_chat(call_data_t *call_data, cJSON *json) {
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    send_message_to_chat(call_data, message_json->valuestring, chat_id);
}

