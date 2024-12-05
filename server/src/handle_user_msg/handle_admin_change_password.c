#include "create_json.h"
#include "password.h"


cJSON *handle_admin_change_password(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "login")
        || !cJSON_HasObjectItem(json, "new_password")) {
        return create_error_json("Invalid data\n");
    }

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    bool is_admin = call_data->client_data->user_data->is_admin;
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    if (!is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_login_json = cJSON_GetObjectItemCaseSensitive(json, "login");
    
    // Critical resource access: LOGIN_TO_ID HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
    int user_to_update_id = ht_str_get(call_data->general_data->login_to_id,
                                       user_login_json->valuestring);
    pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
    // Critical resource access: LOGIN_TO_ID HASH TABLE. End

    if (user_to_update_id == -1 || user_to_update_id == 1) {
        return create_error_json("No such user\n");
    }

    cJSON *new_password_json = cJSON_GetObjectItemCaseSensitive(json, "new_password");

    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    client_t *client_data = ht_get(call_data->general_data->clients, user_to_update_id);
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End

    if (client_data == NULL) {
        return create_error_json("No such user\n");
    }

    // Critical resource access: CLIENT USER DATA. Start
    pthread_mutex_lock(&client_data->user_data->mutex);
    unsigned char *new_hash = hash_password(new_password_json->valuestring,
                                            client_data->user_data->login);
    pthread_mutex_unlock(&client_data->user_data->mutex);
    // Critical resource access: CLIENT USER DATA. End

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int return_code = update_password_hash(call_data->general_data->db, 
                                           user_to_update_id, new_hash);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End
    
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
