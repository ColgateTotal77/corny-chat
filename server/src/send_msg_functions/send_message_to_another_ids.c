#include "server.h"


void send_message_to_another_ids(call_data_t *call_data, char *s) {
    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    
    int count = 0;
    entry_t** entries = ht_dump(call_data->general_data->clients, &count);
    int uid = call_data->client_data->user_data->user_id;
    bool is_online;
    
    for (int i = 0; i < count; i++) {
        entry_t* entry = entries[i];
		if (entry->key == uid) { continue; }
        
        client_t *client_data = (client_t*)entry->value;

        // Critical resource access: SELECTED CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        is_online = client_data->user_data->is_online;
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: SELECTED CLIENT USER DATA. End

        // Critical resource access: SELECTED CLIENT CONNECTION. Start
        pthread_mutex_lock(&client_data->mutex);
        
		if (!is_online || !client_data->ssl) {
            pthread_mutex_unlock(&client_data->mutex);
            // Critical resource access: SELECTED CLIENT CONNECTION. Possible end

            continue;
        }
        
        int write_len = SSL_write(client_data->ssl, s, strlen(s));
        if (write_len <= 0) {
            int err = SSL_get_error(client_data->ssl, write_len);
            fprintf(stderr, "SSL_write failed with error: %d\n", err);
            ERR_print_errors_fp(stderr);
        }

        pthread_mutex_unlock(&client_data->mutex);
        // Critical resource access: SELECTED CLIENT CONNECTION. End

    }
    free(entries);

    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End
}

