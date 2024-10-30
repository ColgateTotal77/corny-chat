#include "server.h"
#include "../libmx/inc/libmx.h"


void send_message_to_id(call_data_t *call_data, char *message, int user_id) {
    pthread_mutex_lock(call_data->clients_mutex);

	client_t *client_data = ht_get(call_data->clients, user_id);

    if (write(client_data->sockfd, message, strlen(message)) < 0) {
		perror("ERROR: write to descriptor failed");	
	}

	pthread_mutex_unlock(call_data->clients_mutex);
}

void send_message_to_user(call_data_t *call_data, char *message) {
    pthread_mutex_lock(call_data->clients_mutex);

    int user_id = call_data->client_data->user_data->user_id;
	client_t *client_data = ht_get(call_data->clients, user_id);

    if (write(client_data->sockfd, message, strlen(message)) < 0) {
		perror("ERROR: write to descriptor failed");	
	}

	pthread_mutex_unlock(call_data->clients_mutex);
}

void send_message_to_another_ids(call_data_t *call_data, char *s) {
	pthread_mutex_lock(call_data->clients_mutex);

	int count = 0;
	int sockfd;
	entry_t** entries = ht_dump(call_data->clients, &count);
    int uid = call_data->client_data->user_data->user_id;

	for (int i = 0; i < count; i++) {
		entry_t* entry = entries[i];
		if (entry->key == uid) { continue; }

		sockfd = ((client_t*)entry->value)->sockfd;

		if (write(sockfd, s, strlen(s)) < 0) {
			perror("ERROR: write to descriptor failed");
			break;
		}
	}
	free(entries);

	pthread_mutex_unlock(call_data->clients_mutex);
}

void send_message_to_chat(call_data_t *call_data, char *message, int chat_id) {
	pthread_mutex_lock(call_data->chats_mutex);

	chat_t* chat_data = ht_get(call_data->chats, chat_id);
	int count = chat_data->users_count;
    int sender_uid = call_data->client_data->user_data->user_id;
    char *sender_name = call_data->client_data->user_data->name;

    char buffer[BUF_SIZE + 32];
    sprintf(buffer, "From %s to chat %s: %s", sender_name, chat_data->name, message);

	for (int i = 0; i < count; i++) {
		client_t* client_data = ht_get(call_data->clients, chat_data->users_id[i]);

		if (client_data->user_data->user_id == sender_uid) { continue; }

        if (write(client_data->sockfd, buffer, strlen(buffer)) < 0) {
	    	perror("ERROR: write to descriptor failed");
	    	break;
	    }

	}

	pthread_mutex_unlock(call_data->chats_mutex);
}

void send_user_exit_msg(call_data_t *call_data) {
    char buffer[BUF_SIZE];
	sprintf(buffer, "%s has left\n", call_data->client_data->user_data->name);
	printf("%s", buffer);
	send_message_to_another_ids(call_data, buffer);
}

