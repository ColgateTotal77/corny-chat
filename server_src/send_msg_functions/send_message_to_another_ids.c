#include "server.h"


void send_message_to_another_ids(call_data_t *call_data, char *s) {
	pthread_mutex_lock(call_data->general_data->clients_mutex);

	int count = 0;
	int sockfd;
	entry_t** entries = ht_dump(call_data->general_data->clients, &count);
    int uid = call_data->client_data->user_data->user_id;

	for (int i = 0; i < count; i++) {
		entry_t* entry = entries[i]; 
		if (entry->key == uid) { continue; }

		client_t *client_data = (client_t*)entry->value;

		if (!(client_data->user_data->is_online) || client_data->sockfd < 0) {
			continue;
		}

		sockfd = client_data->sockfd;

		if (write(sockfd, s, strlen(s)) < 0) {
			perror("ERROR: write to descriptor failed");
			break;
		}
	}
	free(entries);

	pthread_mutex_unlock(call_data->general_data->clients_mutex);
}

