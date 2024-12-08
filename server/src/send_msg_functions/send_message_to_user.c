#include "server.h"

void send_message_to_user(call_data_t *call_data, char *message) {
    printf("MUTEX LOG: send_message_to_user<--------------------------------\n");
    fflush(stdout);

    printf("MUTEX LOG: lock(&call_data->client_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: USER CONNECTION DATA. Start
    pthread_mutex_lock(&call_data->client_data->mutex);
    
    int write_len = SSL_write(call_data->client_data->ssl, message, strlen(message));
    if (write_len <= 0) { 
        fprintf(stderr, "SSL_write failed with error: %d\n", 
                SSL_get_error(call_data->client_data->ssl, write_len));
        ERR_print_errors_fp(stderr);
    }
    
    pthread_mutex_unlock(&call_data->client_data->mutex);
    // Critical resource access: USER CONNECTION DATA. End
    printf("MUTEX LOG: unlock(&call_data->client_data->mutex)\n");
    fflush(stdout);
}

