#include "server.h"


void send_message_to_chat(call_data_t *call_data, char *message, int chat_id) {

    pthread_mutex_lock(call_data->general_data->chats_mutex);
    
    chat_t* chat_data = ht_get(call_data->general_data->chats, chat_id);
    if (!chat_data) {
        fprintf(stderr, "No chat was found with id %d\n", chat_id);
        pthread_mutex_unlock(call_data->general_data->chats_mutex);
        return;
    }

	int count = chat_data->users_count;
    int sender_uid = call_data->client_data->user_data->user_id;

    for (int i = 0; i < count; i++) {
        client_t* client_data = ht_get(call_data->general_data->clients, chat_data->users_id[i]);
        
		if (client_data->user_data->user_id == sender_uid
		    || !(client_data->user_data->is_online)
			|| !client_data->ssl) {
			continue;
		}
        
        int write_len = SSL_write(client_data->ssl, message, strlen(message));
        if (write_len <= 0) {
            int err = SSL_get_error(client_data->ssl, write_len);
            fprintf(stderr, "SSL_write failed with error: %d\n", err);
            ERR_print_errors_fp(stderr);
        }
    }
    
    pthread_mutex_unlock(call_data->general_data->chats_mutex);
}

