#define _XOPEN_SOURCE
#include "server.h"
#include "password.h"
#include "cJSON.h"
#include "sql.h"
#include "time.h"


static enum LoginValidationResult validate_login_via_session_id(call_data_t *call_data,
                                                                cJSON *json) {
    cJSON *session_id = cJSON_GetObjectItemCaseSensitive(json, "session_id");
    
    // Critical resource access: SESSION ID TO ID HASH MAP. Start
    pthread_mutex_lock(call_data->general_data->session_id_to_id_mutex);
    int client_id = ht_str_get(call_data->general_data->session_id_to_id,
                               session_id->valuestring);
    pthread_mutex_unlock(call_data->general_data->session_id_to_id_mutex);
    // Critical resource access: SESSION ID TO ID HASH MAP. End

    if (client_id == -1) {
        printf("No such session id\n");
        fflush(stdout);
        return INVALID_INPUT;
    }

    // Critical resource access: CLIENTS HASH MAP. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    client_t *client_data = ht_get(call_data->general_data->clients, client_id);
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH MAP. End

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&client_data->user_data->mutex);
    bool is_active = client_data->user_data->is_active;
    pthread_mutex_unlock(&client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    if (is_active) {
        printf("User was deactivated\n");
        fflush(stdout);

        // Critical resource access: SESSION ID TO ID HASH MAP. Start
        pthread_mutex_lock(call_data->general_data->session_id_to_id_mutex);
        ht_str_del(call_data->general_data->session_id_to_id, session_id->valuestring);
        pthread_mutex_unlock(call_data->general_data->session_id_to_id_mutex);
        // Critical resource access: SESSION ID TO ID HASH MAP. End

        return INVALID_INPUT;
    }

    struct tm parsed_time;
    
    // Critical resource access: CLIENT DATA. Start
    pthread_mutex_lock(&call_data->client_data->mutex);
    strptime(client_data->time_created_session_id, "%Y-%m-%d %H:%M:%S", &parsed_time);
    pthread_mutex_unlock(&call_data->client_data->mutex);
    // Critical resource access: CLIENT DATA. End

    time_t time_of_session_id_creation = mktime(&parsed_time);

    time_t current_time;
    time(&current_time);

    if (difftime(current_time, time_of_session_id_creation) < (double)(60 * 60 * 5)) {
        return VALID_LOGIN_VIA_SESSION_ID;
    }
    else {
        printf("Session id expired\n");
        fflush(stdout);

        // Critical resource access: SESSION ID TO ID HASH MAP. Start
        pthread_mutex_lock(call_data->general_data->session_id_to_id_mutex);
        ht_str_del(call_data->general_data->session_id_to_id, session_id->valuestring);
        pthread_mutex_unlock(call_data->general_data->session_id_to_id_mutex);
        // Critical resource access: SESSION ID TO ID HASH MAP. End

        return INVALID_INPUT;
    }
}

enum LoginValidationResult find_and_validate_user(call_data_t *call_data, 
                                               cJSON *json_login_password) {
    if (cJSON_HasObjectItem(json_login_password, "session_id")) {
        printf("Trying to validate session id\n");
        fflush(stdout);
        return validate_login_via_session_id(call_data, json_login_password);
	}

    if (!cJSON_HasObjectItem(json_login_password, "name")
        || !cJSON_HasObjectItem(json_login_password, "password")) {
        return INVALID_INPUT;
    }

    cJSON *login = cJSON_GetObjectItemCaseSensitive(json_login_password, "name");
	cJSON *password = cJSON_GetObjectItemCaseSensitive(json_login_password, "password");
    enum LoginValidationResult validation_result;

    if (!check_login(login->valuestring) || !check_password(password->valuestring)) { 
        return INVALID_INPUT;
	}

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    unsigned char *db_hash = get_password_hash(call_data->general_data->db, login->valuestring);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End
    
    unsigned char *input_hash = hash_password(password->valuestring, login->valuestring);


    if (db_hash == NULL) {
        free(input_hash);
        return INVALID_INPUT;
    }

    if (memcmp(db_hash, input_hash, 32) == 0) {
        validation_result = VALID_LOGIN;
    }
    else {
        validation_result = INVALID_INPUT;
    }

    free(db_hash);
    free(input_hash);

    return validation_result;
}

