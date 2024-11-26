#include "create_json.h"
#include "sql.h"


cJSON *handle_activate_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "user_id")) {
        return create_error_json("Invalid json format\n");
    }

    if (!call_data->client_data->user_data->is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_to_activate_id_json = cJSON_GetObjectItemCaseSensitive(json, "user_id");
    int user_to_activate = user_to_activate_id_json->valueint;
    
    if (user_to_activate == call_data->client_data->user_data->user_id) {
        return create_error_json("You can't activate your self. Use another admin account\n");
    }

    client_t *activated_user_data = ht_get(call_data->general_data->clients,
                                           user_to_activate);
    bool is_active = true;

    if (activated_user_data == NULL) {
        cJSON *error_response = create_error_json("No such user\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_activate);
        return error_response;
    }

    if (activated_user_data->user_data->is_active == is_active) {
        cJSON *error_response = create_error_json("Already activated\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_activate);
        return error_response;
    }

    
    activate_deactivate_user(
        call_data->general_data->db,
        user_to_activate, is_active);


    activated_user_data->user_data->is_active = true;

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "user_id", user_to_activate);

    return response_json;
}
