#include "server.h"
#include "cJSON.h"
#include "create_json.h"


cJSON *handle_send_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")
        || !cJSON_HasObjectItem(json, "message")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    send_message_to_chat(call_data, message_json->valuestring, chat_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);

    return response_json;
}

