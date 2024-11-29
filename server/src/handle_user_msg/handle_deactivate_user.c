#include "create_json.h"
#include "sql.h"


cJSON *handle_deactivate_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "login")) {
        return create_error_json("Invalid json format\n");
    }

    if (!call_data->client_data->user_data->is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_login_json = cJSON_GetObjectItemCaseSensitive(json, "login");
    int user_to_deactivate = ht_str_get(call_data->general_data->login_to_id, user_login_json->valuestring);

    if (user_to_deactivate == -1 ||
        user_to_deactivate == 1) {
        return create_error_json("No such user\n");
    }
    
    if (user_to_deactivate == call_data->client_data->user_data->user_id) {
        return create_error_json("You can't deactivate your self. Use another admin account\n");
    }

    client_t *deactivated_user_data = ht_get(call_data->general_data->clients,
                                             user_to_deactivate);
    bool is_active = false;

    if (deactivated_user_data == NULL) {
        cJSON *error_response = create_error_json("No such user\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_deactivate);
        cJSON_AddStringToObject(error_response, "login", user_login_json->valuestring);
        return error_response;
    }

    if (deactivated_user_data->user_data->is_active == is_active) {
        cJSON *error_response = create_error_json("Already activated\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_deactivate);
        cJSON_AddStringToObject(error_response, "login", user_login_json->valuestring);
        return error_response;
    }

    activate_deactivate_user(
        call_data->general_data->db, 
        user_to_deactivate, is_active);

    
    deactivated_user_data->user_data->is_active = false;


    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "user_id", user_to_deactivate);
    cJSON_AddStringToObject(response_json, "login", user_login_json->valuestring);

    return response_json;
}
