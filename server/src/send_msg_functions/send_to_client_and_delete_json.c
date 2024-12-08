#include "server.h"


void send_to_client_and_delete_json(cJSON **json, client_t* client_data) {
    printf("MUTEX LOG: send_to_client_and_delete_json<--------------------------------\n");
    fflush(stdout);
    if (!client_data) {
        return;
    }
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);

    printf("MUTEX LOG: lock(&client_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: CLIENT SSL CONNECTION DATA. Start
    pthread_mutex_lock(&client_data->mutex);

    printf("MUTEX LOG: lock(&client_data->user_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: CLIENT USER DATA. Start
    pthread_mutex_lock(&client_data->user_data->mutex);
    bool is_online = client_data->user_data->is_online;
    pthread_mutex_unlock(&client_data->user_data->mutex);
    // Critical resource access: CLIENT USER DATA. End
    printf("MUTEX LOG: unlock(&client_data->user_data->mutex)\n");
    fflush(stdout);
    
    if (!is_online || !client_data->ssl) {
        fprintf(stderr, "User with id %d is not available or offline\n", 
                client_data->user_data->user_id);
    }
    else {
        int write_len = SSL_write(client_data->ssl, response, strlen(response));
        if (write_len <= 0) {
            fprintf(stderr, "SSL_write failed with error: %d\n", 
                    SSL_get_error(client_data->ssl, write_len));
            ERR_print_errors_fp(stderr);
        }
    }

    pthread_mutex_unlock(&client_data->mutex);
    // Critical resource access: CLIENT SSL CONNECTION DATA. End
    printf("MUTEX LOG: unlock(&client_data->mutex)\n");
    fflush(stdout);

    free(response);
    cJSON_Delete(*json);
}
