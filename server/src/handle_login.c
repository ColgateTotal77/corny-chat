#include "server.h"
#include "command_codes.h"
#include "password.h"

static bool is_active_user(call_data_t *call_data, char *login) {
	// Critical resource access: LOGIN_TO_ID HASH MAP. Start
	pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
	int user_id = ht_str_get(call_data->general_data->login_to_id, login);
    pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
	// Critical resource access: LOGIN_TO_ID HASH MAP. End

	if (user_id == -1) {
		return false;
	}

    // Critical resource access: CLIENTS HASH MAP. Start
	pthread_mutex_lock(call_data->general_data->clients_mutex);
	client_t *client_data = ht_get(call_data->general_data->clients, user_id);
	pthread_mutex_unlock(call_data->general_data->clients_mutex);
	// Critical resource access: CLIENTS HASH MAP. End

	if (client_data == NULL) {
		return false;
	}

    // Critical resource access: CLIENT USER DATA. Start
	pthread_mutex_lock(&client_data->user_data->mutex);
	bool is_active = client_data->user_data->is_active;
	pthread_mutex_unlock(&client_data->user_data->mutex);
	// Critical resource access: CLIENT USER DATA. End

	return is_active;
}

static void update_server_client_data(call_data_t *call_data, int client_id) {
    client_t *client_data = call_data->client_data;
    ht_t *clients = call_data->general_data->clients;
    
	// Critical resource access: CLIENTS HASH MAP. Start
	pthread_mutex_lock(call_data->general_data->clients_mutex);
	client_t *cached_client_data = ht_get(clients, client_id);
	pthread_mutex_unlock(call_data->general_data->clients_mutex);
	// Critical resource access: CLIENTS HASH MAP. End

    // Critical resource access: CLIENT CONNECTION DATA. Start
	pthread_mutex_lock(&cached_client_data->mutex);
	// Critical resource access: CLIENT USER DATA. Start
	pthread_mutex_lock(&cached_client_data->user_data->mutex);
	cached_client_data->ssl = client_data->ssl;
	cached_client_data->socket = client_data->socket;
	cached_client_data->user_data->is_online = true;
    pthread_mutex_unlock(&cached_client_data->user_data->mutex);
	// Critical resource access: CLIENT USER DATA. End
	pthread_mutex_unlock(&cached_client_data->mutex);
	// Critical resource access: CLIENT CONNECTION DATA. End

	free(client_data);
	call_data->client_data = cached_client_data;
}

static void handle_valid_login_via_session_id(call_data_t *call_data, 
                                              cJSON *json_login_password) {
    cJSON *session_id = cJSON_GetObjectItemCaseSensitive(json_login_password, "session_id");
    
    // Critical resource access: LOGIN_TO_ID HASH MAP. Start
	pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
    int client_id = ht_str_get(call_data->general_data->session_id_to_id,
                               session_id->valuestring);
	pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
    // Critical resource access: LOGIN_TO_ID HASH MAP. End

    update_server_client_data(call_data, client_id);
	send_user_returned_msg(call_data);
}

static void create_and_send_succesfull_login_data(call_data_t *call_data,
                                                  int user_id, char* login,
                                                  char *client_session_id) {
    cJSON *response_json = create_response_json(LOGIN, true, NULL);
	cJSON_AddNumberToObject(response_json, "user_id", user_id);
	cJSON_AddStringToObject(response_json, "login", login);
	cJSON_AddStringToObject(response_json, "nickname",
	                        call_data->client_data->user_data->nickname);
	cJSON_AddStringToObject(response_json, "session_id", client_session_id);
	cJSON_AddBoolToObject(response_json, "is_admin", call_data->client_data->user_data->is_admin);
    send_to_user_and_delete_json(call_data, &response_json);
}

static char *set_session_id_data(call_data_t *call_data, int user_id) {
    char *client_session_id = generate_long_salt();
    
	ht_str_t *session_id_to_id = call_data->general_data->session_id_to_id;

	// Critical resource access: SESSION ID TO ID HASH MAP. Start
    pthread_mutex_lock(call_data->general_data->session_id_to_id_mutex);
	while (ht_str_get(session_id_to_id, client_session_id) != -1) {
		printf("Regenerating session id\n");
		fflush(stdout);
		free(client_session_id);
        client_session_id = generate_long_salt();
	}

    ht_str_set(session_id_to_id, client_session_id, user_id);
	pthread_mutex_unlock(call_data->general_data->session_id_to_id_mutex);
    // Critical resource access: SESSION ID TO ID HASH MAP. End
	
	char *time_str = get_string_time();

    // Critical resource access: CLIENT DATA. Start
	pthread_mutex_lock(&call_data->client_data->mutex);
	strcpy(call_data->client_data->time_created_session_id, time_str);
    pthread_mutex_unlock(&call_data->client_data->mutex);
	// Critical resource access: CLIENT DATA. End

	free(time_str);

	return client_session_id;
}

static void handle_valid_login(call_data_t *call_data, char *login) {
	ht_str_t *login_to_id = call_data->general_data->login_to_id;

	// Critical resource access: LOGIN_TO_ID HASH MAP. Start
	pthread_mutex_lock(call_data->general_data->login_to_id_mutex);
	int user_id = ht_str_get(login_to_id, login);
    pthread_mutex_unlock(call_data->general_data->login_to_id_mutex);
    // Critical resource access: LOGIN_TO_ID HASH MAP. End
	
	update_server_client_data(call_data, user_id);
    
	char *client_session_id = set_session_id_data(call_data, user_id);
	create_and_send_succesfull_login_data(call_data, user_id, login, client_session_id);
	free(client_session_id);

	send_user_returned_msg(call_data);
}

int handle_login(char *str_json_login_password, call_data_t *call_data) {
    cJSON *json_login_password = cJSON_Parse(str_json_login_password);
	cJSON *login = cJSON_GetObjectItemCaseSensitive(json_login_password, "name");

	enum LoginValidationResult is_valid_login;
	is_valid_login = find_and_validate_user(call_data, json_login_password);

    int leave_flag = 0;
   
	if (is_valid_login == INVALID_INPUT || !is_active_user(call_data, login->valuestring)) {
        printf("Invalid Input\n");
		cJSON *response_json = create_response_json(LOGIN, false, "Invalid login input\n");
		send_to_user_and_delete_json_no_mutexes(call_data, &response_json);

		SSL_shutdown(call_data->client_data->ssl); //Коректне завершення SSL-сесії
        SSL_free(call_data->client_data->ssl);
        call_data->client_data->ssl = NULL;
		free(call_data->client_data);
		call_data->client_data = NULL;

		leave_flag = 1;
	}
	else if (is_valid_login == VALID_LOGIN_VIA_SESSION_ID) {
        printf("Valid Login via session id\n");
        handle_valid_login_via_session_id(call_data, json_login_password);
	}
	else if (is_valid_login == VALID_LOGIN) {
        printf("Valid Login\n");
        handle_valid_login(call_data, login->valuestring);
	}

	cJSON_Delete(json_login_password);

    return leave_flag;
}

