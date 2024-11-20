#include "server.h"


void send_user_exit_msg(call_data_t *call_data) {
    char buffer[BUF_SIZE];
	sprintf(buffer, "%s has left\n", call_data->client_data->user_data->login);
	printf("%s", buffer);

	char *response = add_length_to_string(buffer);

	send_message_to_another_ids(call_data, response);
    
    free(response);
}

