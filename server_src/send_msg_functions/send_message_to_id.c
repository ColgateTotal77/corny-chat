#include "server.h"


void send_message_to_id(call_data_t *call_data, char *message, int user_id) {
    pthread_mutex_lock(call_data->general_data->clients_mutex);

	client_t *client_data = ht_get(call_data->general_data->clients, user_id);

	if (client_data == NULL) {
		printf("No user was found with id %d\n", user_id);
		fflush(stdout);
	}
	else if (!client_data->user_data->is_online || client_data->sockfd < 0) {
		printf("Can not notify user, because he is offlline\n");
		fflush(stdout);
	}
    else if (write(client_data->sockfd, message, strlen(message)) < 0) {
		perror("ERROR: write to descriptor failed");	
	}

	pthread_mutex_unlock(call_data->general_data->clients_mutex);
}

