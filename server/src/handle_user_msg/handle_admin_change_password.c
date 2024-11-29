#include "create_json.h"
#include "password.h"


cJSON *handle_admin_change_password(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "login")
        || !cJSON_HasObjectItem(json, "new_password")) {
        return create_error_json("Invalid data\n");
    }

    if (!call_data->client_data->user_data->is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_login_json = cJSON_GetObjectItemCaseSensitive(json, "login");
    int user_to_update_id = ht_str_get(call_data->general_data->login_to_id,
                                       user_login_json->valuestring);

    if (user_to_update_id == -1 ||
        user_to_update_id == 1) {
        return create_error_json("No such user\n");
    }

    cJSON *new_password_json = cJSON_GetObjectItemCaseSensitive(json, "new_password");

    client_t *client_data = ht_get(call_data->general_data->clients, user_to_update_id);

    if (client_data == NULL) {
        return create_error_json("No such user\n");
    }

    unsigned char *new_hash = hash_password(new_password_json->valuestring,
                                            client_data->user_data->login);

    int return_code = update_password_hash(call_data->general_data->db, 
                                           user_to_update_id, new_hash);
    free(new_hash);

    bool success = (return_code == SQLITE_DONE);

    if (!success) {
        return create_error_json("Something went wrong\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddStringToObject(response_json, "login", user_login_json->valuestring);

    return response_json;
}
