#include "create_json.h"


cJSON *handle_mark_chat_msgs_as_readed(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "sender_id")) {
        return create_error_json("Invalid json format\n");
    }

    int user_id = call_data->client_data->user_data->user_id;
    cJSON *sender_id_json = cJSON_GetObjectItemCaseSensitive(json, "sender_id");
    int sender_id = (int)cJSON_GetNumberValue(sender_id_json);

    int setted_messages = set_mes_read_status(call_data->general_data->db, user_id, sender_id);

    if (setted_messages == -1) {
        return create_error_json("Something went wrong\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "sender_id", sender_id);

    return response_json;
}
