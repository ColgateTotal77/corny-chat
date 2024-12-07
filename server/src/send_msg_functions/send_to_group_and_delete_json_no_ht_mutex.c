#include "server.h"


void send_to_group_and_delete_json_no_ht_mutex(call_data_t *call_data, 
                                               cJSON **json, chat_t *chat) {
    printf("MUTEX LOG: send_to_group_and_delete_json_no_ht_mutex"
           "<--------------------------------\n");
    fflush(stdout);
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);
    
	int count = chat->users_count;
    int sender_uid = call_data->client_data->user_data->user_id;
    bool can_send;

    for (int i = 0; i < count; i++) {
        client_t* client_data = ht_get(call_data->general_data->clients, chat->users_id[i]);

        printf("MUTEX LOG: lock(&client_data->user_data->mutex)\n");
        fflush(stdout);
        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        can_send = (!(client_data->user_data->user_id == sender_uid)
		            && client_data->user_data->is_online);
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End
        printf("MUTEX LOG: unlock(&client_data->user_data->mutex)\n");
        fflush(stdout);

        printf("MUTEX LOG: lock(&client_data->mutex)\n");
        fflush(stdout);
        // Critical resource access: CLIENT CONNECTION. Start
        pthread_mutex_lock(&client_data->mutex);
		if (!can_send || !client_data->ssl) {
            pthread_mutex_unlock(&client_data->mutex);
            // Critical resource access: CLIENT CONNECTION. Possible end
            printf("MUTEX LOG: unlock(&client_data->mutex)\n");
            fflush(stdout);

			continue;
		}

        int write_len = SSL_write(client_data->ssl, response, strlen(response));
        if (write_len <= 0) {
            int err = SSL_get_error(client_data->ssl, write_len);
            fprintf(stderr, "SSL_write failed with error: %d\n", err);
            ERR_print_errors_fp(stderr);
        }
        pthread_mutex_unlock(&client_data->mutex);
        // Critical resource access: CLIENT CONNECTION. End
        printf("MUTEX LOG: unlock(&client_data->mutex)\n");
        fflush(stdout);
    }

    free(response);
    cJSON_Delete(*json);
}
