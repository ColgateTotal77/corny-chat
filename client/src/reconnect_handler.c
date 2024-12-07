#include "GTK.h"

void* reconnect_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;
    char *session_id = GTK_data->session_id;

    sleep(3);
    bool session_expired = false;
    SSL* new_ssl = NULL;
    // *(GTK_data->stop_reconnect) = true;
    pthread_mutex_lock(&GTK_data->pthread_mutex);

    while (!GTK_data->stop_reconnect) {

        new_ssl = try_to_reconnect(session_id, call_data->host, 
                                        call_data->port, &session_expired);

    }
    printf("1\n");
    //new_ssl!=NULL && session_expired = false - successful
    //session_expired = true close all open login
    if (new_ssl == NULL) {
        printf("SSL = NULL\nServer disconnected!\n");
        if(session_id){
            free(session_id);
            session_id = NULL;
        }
        *(call_data->stop_flag) = true;
        // break;

    }
    
    else if(session_expired == true) {
        //loginform
    }

    else{
        printf("\nConnection recover!\n");
        SSL_free(call_data->ssl);
        call_data->ssl = new_ssl;
        // continue;
    }

    printf("Stop thread\n");

    pthread_cond_signal(&GTK_data->pthread_cond);
    pthread_mutex_unlock(&GTK_data->pthread_mutex);
    pthread_detach(pthread_self());
    return NULL;
}
