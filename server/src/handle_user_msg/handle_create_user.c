#include "sending_functions.h"
#include "sql.h"
#include "password.h"
#include "command_codes.h"

static bool check_login_validity(char *login) {
    if (login == NULL) {
        return false;
    }

    int login_length = mx_strlen(login);
    if (login_length < 3 || login_length > 39) {
        return false;
    }

    for (int i = 0; i < login_length; i++) {
        if (mx_check_space(login[i]) || !((login[i] >= '0' && login[i] <= '9') || 
        (login[i] >= 'A' && login[i] <= 'Z') || (login[i] >= 'a' && login[i] <= 'z')
        || login[i] == '_')) {

            return false;
        }
    }
    return true;
}


static bool user_already_exists(call_data_t *call_data, char *login) {
    unsigned char *db_hash = get_password_hash(call_data->general_data->db, login);

    // Critical resource access: LOGIN_TO_ID HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
    int user_id = ht_str_get(call_data->general_data->login_to_id, login);
    pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
    // Critical resource access: LOGIN_TO_ID HASH TABLE. End

    if (user_id != -1 || db_hash != NULL) {
        free(db_hash);
        return true;
    }
    return false;
}

static cJSON *create_json_notif_about_new_user(int user_id, char *nickname) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "event_code", NEW_USER_WAS_CREATED);
    cJSON_AddNumberToObject(json, "id", user_id);
    cJSON_AddStringToObject(json, "nickname", nickname);

    return json;
}


cJSON *handle_create_user(call_data_t *call_data, cJSON *json) {
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    bool user_has_rights = (call_data->client_data->user_data->is_active
                           && call_data->client_data->user_data->is_admin);
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    if (!user_has_rights
        || !cJSON_HasObjectItem(json, "login")
        || !cJSON_HasObjectItem(json, "password")
        || !cJSON_HasObjectItem(json, "is_admin")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *login_json = cJSON_GetObjectItemCaseSensitive(json, "login");
    cJSON *password_json = cJSON_GetObjectItemCaseSensitive(json, "password");
    cJSON *is_admin_json = cJSON_GetObjectItemCaseSensitive(json, "is_admin");

    if (!check_login_validity(login_json->valuestring) 
        || !check_password(password_json->valuestring)) {
        return create_error_json("Invalid login or password\n");
    }

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    if (user_already_exists(call_data, login_json->valuestring)) {
        pthread_mutex_unlock(call_data->general_data->db_mutex);
        // Critical resource access: DATABASE. Possible end

        return create_error_json("User already exists\n");
    }

    unsigned char *input_hash = hash_password(password_json->valuestring, 
                                              login_json->valuestring);
    int user_id = create_new_user_and_return_id(call_data, login_json->valuestring,
                                                input_hash, is_admin_json->valueint);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End


    free(input_hash);

    if (user_id == -1) {
        return create_error_json("Something went wrong\n");
    }

    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);

    // Critical resource access: LOGIN_TO_ID HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
    add_offline_user_to_server_cache(NULL,
                                     call_data->general_data->clients,
                                     call_data->general_data->login_to_id,
                                     user_id, login_json->valuestring, NULL,
                                     is_admin_json->valueint, true
    );
    pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
    // Critical resource access: LOGIN_TO_ID HASH TABLE. End

    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End

    cJSON *notif_about_new_user = create_json_notif_about_new_user(user_id, 
                                                                   login_json->valuestring);
    send_to_another_ids_and_delete_json(call_data, &notif_about_new_user);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
	cJSON_AddNumberToObject(response_json, "user_id", user_id);
	cJSON_AddStringToObject(response_json, "login", login_json->valuestring);
	cJSON_AddBoolToObject(response_json, "is_admin", is_admin_json->valueint);
    
    return response_json;
}
