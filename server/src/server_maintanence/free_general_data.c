#include "server.h"

static void free_chats_cache(ht_t *chats) {
    int chats_count = 0;
	entry_t **chat_hash_slots = ht_dump(chats, &chats_count);

	for (int i = 0; i < chats_count; i++) {
		entry_t *entry = chat_hash_slots[i];
        chat_t *chat_data = entry->value;
		pthread_mutex_destroy(&chat_data->mutex);
		free(chat_data->users_id);
		free(chat_data);
		free(entry);
	}
	free(chat_hash_slots);
	free(chats->entries);
	free(chats);
}

static void free_clients_cache(ht_t *clients) {
    int cached_clients_count = 0;
	entry_t **clients_hash_slots = ht_dump(clients, &cached_clients_count);

	for (int i = 0; i < cached_clients_count; i++) {
		entry_t *entry = clients_hash_slots[i];
        client_t *client_data = entry->value;
		pthread_mutex_destroy(&client_data->user_data->mutex);
		free(client_data->user_data->contacts_id);
		free(client_data->user_data->groups_id);
		free(client_data->user_data);
		if (client_data->ssl) {
			SSL_shutdown(client_data->ssl);
            SSL_free(client_data->ssl);
	    }
		//shutdown(client_data->sockfd, SHUT_RDWR);
		pthread_mutex_destroy(&client_data->mutex);
		free(client_data);
		free(entry);
	}
    free(clients_hash_slots);
	free(clients->entries);
	free(clients);
}

void free_general_data(general_data_t *general_data) {
    sqlite3_close(general_data->db);
	free_chats_cache(general_data->chats);
	free_clients_cache(general_data->clients);
    delete_str_table(&(general_data->login_to_id));
	delete_str_table(&(general_data->session_id_to_id));

    free(general_data);
}

