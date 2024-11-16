#include "server.h"


void send_message_to_id(call_data_t *call_data, char *message, int user_id) {

    pthread_mutex_lock(call_data->general_data->clients_mutex);

    client_t *client_data = ht_get(call_data->general_data->clients, user_id);
    
    if (!client_data || !client_data->user_data->is_online || !client_data->ssl) {
        fprintf(stderr, "User with id %d is not available or offline\n", user_id);
    } else {
        int write_len = SSL_write(client_data->ssl, message, strlen(message));
        if (write_len <= 0) {
            fprintf(stderr, "SSL_write failed with error: %d\n", SSL_get_error(client_data->ssl, write_len));
            ERR_print_errors_fp(stderr);
        }
    }
    
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
}

