#include "server.h"
#include "cJSON.h"
#include "create_json.h"
#include "command_codes.h"

static cJSON *create_incoming_group_message_json(char *message, 
                                                 int sender_id,
                                                 char *sender_nickname,
                                                 int chat_id,
                                                 char *chat_name) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "event_code", INCOMING_MESSAGE);
    cJSON_AddNumberToObject(message_json, "message_type", GROUP_MESSAGE);
    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddNumberToObject(message_json, "chat_id", chat_id);
    cJSON_AddStringToObject(message_json, "message", message);
    cJSON_AddStringToObject(message_json, "sender_nickname", sender_nickname);
    cJSON_AddStringToObject(message_json, "chat_name", chat_name);


    return message_json;
}

cJSON *handle_send_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")
        || !cJSON_HasObjectItem(json, "message")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);

    cJSON *message_data_json = create_incoming_group_message_json(
        message_json->valuestring,
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        chat_id,
        chat->name
    );
    send_to_chat_and_delete_json(call_data, &message_data_json, chat_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

