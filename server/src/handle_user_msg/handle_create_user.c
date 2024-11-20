#include "create_json.h"
#include "password.h"


static bool user_already_exists(call_data_t *call_data, char *login) {
    unsigned char *db_hash = get_password_hash(call_data->general_data->db, login);
    int user_id = ht_str_get(call_data->general_data->login_to_id, login);

    if (user_id != -1 || db_hash != NULL) {
        free(db_hash);
        return true;
    }
    return false;
}

cJSON *handle_create_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "login")
        || !cJSON_HasObjectItem(json, "password")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *login_json = cJSON_GetObjectItemCaseSensitive(json, "login");
    cJSON *password_json = cJSON_GetObjectItemCaseSensitive(json, "password");

    if (user_already_exists(call_data, login_json->valuestring)) {
        return create_error_json("User already exists\n");
    }
    
    unsigned char *input_hash = hash_password(password_json->valuestring, login_json->valuestring);
    int user_id = create_new_user_and_return_id(call_data, login_json->valuestring, input_hash);
    free(input_hash);

    add_offline_user_to_server_cache(call_data->general_data->db,
                                     call_data->general_data->clients,
                                     call_data->general_data->login_to_id,
                                     user_id, login_json->valuestring, NULL);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
	cJSON_AddNumberToObject(response_json, "user_id", user_id);
	cJSON_AddStringToObject(response_json, "login", login_json->valuestring);
	cJSON_AddBoolToObject(response_json, "is_admin", true);
    
    return response_json;
}
