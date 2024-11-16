#include "create_json.h"


cJSON *handle_update_nickname(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "new_nickname")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *new_nickname_json = cJSON_GetObjectItemCaseSensitive(json, "new_nickname");
    int user_id = call_data->client_data->user_data->user_id;

    int return_code = update_nickname(call_data->general_data->db, user_id,
                                   new_nickname_json->valuestring);

    bool success = (return_code == SQLITE_DONE);

    if (!success) {
        return create_error_json("Something went wrong\n");
    }

    bzero(call_data->client_data->user_data->nickname,
         sizeof(call_data->client_data->user_data->nickname));
    strcpy(call_data->client_data->user_data->nickname,
           new_nickname_json->valuestring);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddStringToObject(response_json, "new_nickname", new_nickname_json->valuestring);

    return response_json;
}
