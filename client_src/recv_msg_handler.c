#include "GTK.h"

void str_overwrite_stdout(void) {
    printf("%s", "> ");
    fflush(stdout);
}

void* recv_msg_handler(void* arg, GtkWidget *messages_container) {
    call_data_t* call_data = (call_data_t*)arg;
    char message[1024];

    while (!*(call_data->stop_flag)) {
        int bytes_received = SSL_read(call_data->ssl, message, 1024);  // Используем SSL для чтения

        if (bytes_received > 0) {
            add_message(messages_container, message, "10:00", false);
            printf("%s", message);
            str_overwrite_stdout();
        }
        else if (bytes_received == 0) {
            printf("\nServer disconnected\n");
            //*(call_data->stop_flag) = true;
            fflush(stdout);
            break;
        }
        
        else {
            int err = SSL_get_error(call_data->ssl, bytes_received);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                continue;
            }
            fprintf(stderr, "SSL_read failed with error: %d\n", err);
            //*(call_data->stop_flag) = true;
            break;
        }

        memset(message, 0, sizeof(message));
    }

    pthread_detach(pthread_self());
    
    return NULL;
}

