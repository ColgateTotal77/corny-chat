#include "GTK.h"

#include "client.h"
#include <ctype.h>



/**
 * @brief Same as setup_client_socket, but instead of exiting program on error
 *        it just returns -1
 *
 * @param hostname host to connect to
 * @param port port to connect to
 * @return socket number or -1 in case of an errors
 */
static int setup_client_socket_with_silent_errors(char *hostname, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 
    if (sock < 0) {
        return -1;
    }
    
    struct hostent *server = gethostbyname(hostname);
    
    if (server == NULL) {
        return -1;
    }

    struct sockaddr_in serv_addr;

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, (server->h_addr_list)[0], server->h_length);
    serv_addr.sin_port = htons(port);
 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }


    return sock;
}

static SSL_CTX *init_ssl_context(void) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    const SSL_METHOD *method = TLS_client_method();

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // Set minimum TLS version
    //SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    return ctx;
}


/**
 * @brief Tries to create new connection to server
 *
 * @param hostname host to connect to
 * @param port port to connect to
 * @return On success new SSL connection, elsewise NULL
 */
SSL *setup_new_connection(char *host, int port) {
    int socket = setup_client_socket_with_silent_errors(host, port);
    if (socket < 0) {
        return NULL;
    }

    SSL_CTX *ctx = init_ssl_context();
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket);
    if (SSL_connect(ssl) != 1) { 
        return NULL;
    }

    return ssl;
}


void* recv_login_msg_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;
    cJSON *parsed_json;

    bool stop_login = false;

    while (!stop_login) {
        char* message = NULL;
        int bytes_received = recieve_next_response(call_data->ssl, &message);

        if (bytes_received > 0) {
            printf("\nLOGIN message = %s\n",message);
            parsed_json = cJSON_Parse(message);
            if (!parsed_json) {
                continue;
            }
            cJSON *command_code_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "command_code");
            if (command_code_json) {
                if ((command_code_json)->valueint == 11) {
                    pthread_mutex_lock(&GTK_data->login_mutex);
                    if(cJSON_GetObjectItemCaseSensitive(parsed_json, "success")->valueint > 0) {
                        cJSON *session_id_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "session_id");
                        GTK_data->session_id = (char*)calloc(strlen(session_id_json->valuestring)+ 1, sizeof(char));
                        strncpy(GTK_data->session_id, session_id_json->valuestring, strlen(session_id_json->valuestring));
                        GTK_data->user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                        char* nickname = cJSON_GetObjectItemCaseSensitive(parsed_json, "nickname")->valuestring;
                        GTK_data->username = (char*)calloc(strlen(nickname)+ 1, sizeof(char));
                        strncpy(GTK_data->username, nickname, strlen(nickname));

                        GTK_data->login_successful = true;
                        stop_login = true;
                    }else{
                        call_data->ssl = setup_new_connection(call_data->host, call_data->port);
                    }
                    
                    pthread_cond_signal(&GTK_data->login_cond);
                    pthread_mutex_unlock(&GTK_data->login_mutex);
                    cJSON_Delete(parsed_json);
                    free(message);
                    continue;
                }
            }
            cJSON_Delete(parsed_json);
        }
        free(message);
    }
    printf("pthread_detach(pthread_self());\n");
    pthread_detach(pthread_self());
    return NULL;
}
