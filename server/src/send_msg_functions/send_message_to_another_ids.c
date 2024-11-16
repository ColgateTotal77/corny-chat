#include "server.h"


void send_message_to_another_ids(call_data_t *call_data, char *s) {

    pthread_mutex_lock(call_data->general_data->clients_mutex);
    
    int count = 0;
    entry_t** entries = ht_dump(call_data->general_data->clients, &count);
    int uid = call_data->client_data->user_data->user_id;
    
    for (int i = 0; i < count; i++) {
        entry_t* entry = entries[i];
		if (entry->key == uid) { continue; }
        
        client_t *client_data = (client_t*)entry->value;

		if (!(client_data->user_data->is_online) || !client_data->ssl) {
            continue;
        }
        
        int write_len = SSL_write(client_data->ssl, s, strlen(s));
        if (write_len <= 0) {
            int err = SSL_get_error(client_data->ssl, write_len);
            fprintf(stderr, "SSL_write failed with error: %d\n", err);
            ERR_print_errors_fp(stderr);
        }
    }
    free(entries);

    pthread_mutex_unlock(call_data->general_data->clients_mutex);
}

