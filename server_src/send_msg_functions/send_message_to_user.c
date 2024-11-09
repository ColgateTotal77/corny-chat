#include "server.h"


void send_message_to_user(call_data_t *call_data, char *message) {
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    

    if (write(call_data->client_data->sockfd, message, strlen(message)) < 0) {
		perror("ERROR: write to descriptor failed");	
	}

	pthread_mutex_unlock(call_data->general_data->clients_mutex);
}

