#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_join_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    int user_id = call_data->client_data->user_data->user_id;

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);

    append_to_intarr(&chat->users_id, &chat->users_count, user_id);
    append_to_intarr(&call_data->client_data->user_data->groups_id,
                     &call_data->client_data->user_data->groups_count, chat_id);

    char buffer[BUF_SIZE];
    sprintf(buffer, "Succesfully joined to chat %s\n", chat->name);
    send_message_to_user(call_data, buffer);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_joined", chat_id);
    cJSON_AddStringToObject(response_json, "system_message", buffer);

    return response_json;
}

