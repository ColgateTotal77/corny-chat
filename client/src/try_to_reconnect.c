#include "client.h"
#include <ctype.h>
#include "commands.h"


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

static bool is_login_data_accepted(SSL *ssl) {
    char* message = NULL;
    int bytes_received = recieve_next_response(ssl, &message);
    
    if (bytes_received > 0) {
        cJSON *parsed_json = cJSON_Parse(message);
        cJSON *success_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "success");
        if (success_json->valueint == 1) {
            cJSON_Delete(parsed_json);
            free(message);
            return true;
        }
        cJSON_Delete(parsed_json);
    }

    free(message);

    return false;
}

/**
 * @brief Tries to create new connection to server for 30 times sleeping 1 second after
 *        each attempt, if succeeds in connect sends session_id to server in order to login again
 * @warning this function is not tested yet
 *
 * @param session_id session_id given by server in response to login
 * @param hostname host to connect to
 * @param port port to connect to
 * @return On success new SSL connection, elsewise NULL
 */
SSL *try_to_reconnect(char *session_id, char *host, int port, bool *session_expired) {
    if (session_id == NULL || host == NULL) {
        return NULL;
    }
    //int count = 0;
    SSL *new_ssl = NULL;

//  while (new_ssl == NULL && stoper) {
        printf("Trying to reconnect\n");
        // count += 1;
        int socket = setup_client_socket_with_silent_errors(host, port);
        if (socket < 0) {
            return NULL;
        }
        SSL_CTX *ctx = init_ssl_context();
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, socket);
        if (SSL_connect(ssl) != 1) { 
            return NULL;
        // sleep(1);
        // continue;
    }
    new_ssl = ssl;
    // }

    cJSON *reconect_request = cJSON_CreateObject();
    cJSON_AddStringToObject(reconect_request, "session_id", session_id);

    if (new_ssl != NULL) {
        send_and_delete_json(new_ssl, &reconect_request);

        if (is_login_data_accepted(new_ssl)) {
            *session_expired = false;
            return new_ssl;
        }

        *session_expired = true;
        if (session_expired) {
            printf("\nsession_expired true!!!!\n\n");
        }
        SSL_shutdown(new_ssl);//Закриття SSL з'єднання
        SSL_free(new_ssl);
    }
    return NULL;
}
