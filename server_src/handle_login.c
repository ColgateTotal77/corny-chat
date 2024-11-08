#include "server.h"


static void handle_no_such_user(call_data_t *call_data, int user_id, char *name) {
    char buff_out[BUF_SIZE];
    client_t *client_data = call_data->client_data;
    ht_t *clients = call_data->general_data->clients;
    
    user_t *user_data = init_user_data(user_id, name, true);
	client_data->user_data = user_data;
    ht_set(clients, user_data->user_id, (void*)client_data);
    ht_str_set(call_data->general_data->login_to_id, name, user_id);

	sprintf(buff_out, "%s has joined\n", name);
	printf("%s", buff_out);
	send_message_to_another_ids(call_data, buff_out);
}

static void handle_valid_login(call_data_t *call_data, char *name) {
    char buff_out[BUF_SIZE];
    client_t *client_data = call_data->client_data;
    ht_t *clients = call_data->general_data->clients;
    ht_str_t *login_to_id = call_data->general_data->login_to_id;

    int user_id = ht_str_get(login_to_id, name);

	client_t *cached_client_data = ht_get(clients, user_id);
	cached_client_data->sockfd = client_data->sockfd;
	cached_client_data->address = client_data->address;
	cached_client_data->user_data->is_online = true;
	free(client_data);
	call_data->client_data = cached_client_data;

	sprintf(buff_out, "%s has returned\n", name);
	printf("%s", buff_out);
	send_message_to_another_ids(call_data, buff_out);
}

int handle_login(char *str_json_name_password, call_data_t *call_data) {
    cJSON *json_name_password = cJSON_Parse(str_json_name_password);
	cJSON *name = cJSON_GetObjectItemCaseSensitive(json_name_password, "name");
    int user_id = -1;

	enum LoginValidationResult is_valid_login;
	is_valid_login = find_or_create_user(call_data, json_name_password, &user_id);

    int leave_flag = 0;
   
	if (is_valid_login == INVALID_INPUT) {
        printf("Invalid Input\n");
        send_message_to_user(call_data, "Invalid login input\n");
		free(call_data->client_data);
		call_data->client_data = NULL;
		leave_flag = 1;
	}
	else if (is_valid_login == NO_SUCH_USER) {
        printf("No such user\n");
		handle_no_such_user(call_data, user_id, name->valuestring);
	}
	else if (is_valid_login == VALID_LOGIN) {
        printf("Valid Login\n");
        handle_valid_login(call_data, name->valuestring);
	}

	cJSON_Delete(json_name_password);

    return leave_flag;
}

