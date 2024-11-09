#include "server.h"


void send_message_to_chat(call_data_t *call_data, char *message, int chat_id) {
	pthread_mutex_lock(call_data->general_data->chats_mutex);

	chat_t* chat_data = ht_get(call_data->general_data->chats, chat_id);

    if (chat_data == NULL) {
		printf("No chat was found with id %d", chat_id);
		fflush(stdout);
		pthread_mutex_unlock(call_data->general_data->chats_mutex);
		return;
	}

	int count = chat_data->users_count;
    int sender_uid = call_data->client_data->user_data->user_id;
    char *sender_name = call_data->client_data->user_data->name;

    char buffer[BUF_SIZE + 32];
    sprintf(buffer, "From %s to chat %s: %s", sender_name, chat_data->name, message);

	for (int i = 0; i < count; i++) {
		client_t* client_data = ht_get(call_data->general_data->clients, chat_data->users_id[i]);

		if (client_data->user_data->user_id == sender_uid
		    || !(client_data->user_data->is_online)
			|| client_data->sockfd < 0) {
			continue;
		}

        if (write(client_data->sockfd, buffer, strlen(buffer)) < 0) {
	    	perror("ERROR: write to descriptor failed");
	    	break;
	    }
	}

	pthread_mutex_unlock(call_data->general_data->chats_mutex);
}

