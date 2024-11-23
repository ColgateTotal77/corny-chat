#include "create_json.h"
#include "password.h"


cJSON *handle_admin_change_password(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "user_id")
        || !cJSON_HasObjectItem(json, "new_password")) {
        return create_error_json("Invalid data\n");
    }

    if (!call_data->client_data->user_data->is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_id_json = cJSON_GetObjectItemCaseSensitive(json, "user_id");
    cJSON *new_password_json = cJSON_GetObjectItemCaseSensitive(json, "new_password");

    client_t *client_data = ht_get(call_data->general_data->clients, user_id_json->valueint);

    unsigned char *new_hash = hash_password(new_password_json->valuestring,
                                            client_data->user_data->login);

    int return_code = update_password_hash(call_data->general_data->db, 
                                           user_id_json->valueint, new_hash);
    free(new_hash);

    bool success = (return_code == SQLITE_DONE);

    if (!success) {
        return create_error_json("Something went wrong\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);

    return response_json;
}
