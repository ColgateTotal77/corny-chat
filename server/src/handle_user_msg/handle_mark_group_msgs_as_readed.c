#include "create_json.h"


cJSON *handle_mark_group_msgs_as_readed(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")) {
        return create_error_json("Invalid json format\n");
    }

    int user_id = call_data->client_data->user_data->user_id;
    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    int group_id = (int)cJSON_GetNumberValue(group_id_json);

    if (!group_exists(call_data, group_id)) {
        cJSON *error_response = create_error_json("No such group\n");
        cJSON_AddNumberToObject(error_response, "group_id", group_id);
        return error_response;
    }

    int setted_messages = set_group_mes_read_status(call_data->general_data->db, user_id, group_id);

    if (setted_messages == -1) {
        return create_error_json("Something went wrong\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "group_id", group_id);

    return response_json;
}
