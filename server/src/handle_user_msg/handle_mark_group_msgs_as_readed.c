#include "create_json.h"


cJSON *handle_mark_group_msgs_as_readed(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")) {
        return create_error_json("Invalid json format\n");
    }

    int user_id = call_data->client_data->user_data->user_id;
    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    int group_id = (int)cJSON_GetNumberValue(group_id_json);
    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, group_id, &chat
    );

    if (!chat_retrieved_successfully) {
        cJSON *error_response = create_error_json("No such group\n");
        cJSON_AddNumberToObject(error_response, "group_id", group_id);
        return error_response;
    }

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int setted_messages = set_group_mes_read_status(call_data->general_data->db, 
                                                    user_id, group_id);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    if (setted_messages == -1) {
        return create_error_json("Something went wrong\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "group_id", group_id);

    return response_json;
}
