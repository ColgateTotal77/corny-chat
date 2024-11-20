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
    printf ("Creating socket...\n");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 
    if (sock < 0) {
        return -1;
    }
    
    printf ("Looking for the host...\n");
    struct hostent *server = gethostbyname(hostname);
    
    if (server == NULL) {
        return -1;
    }

    struct sockaddr_in serv_addr;

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, (server->h_addr_list)[0], server->h_length);
    serv_addr.sin_port = htons(port);
    printf ("Connecting to server...\n");
 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    printf("Client started\n");

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
 * @brief Tries to create new connection to server for 30 times sleeping 1 second after
 *        each attempt, if succeeds in connect sends session_id to server in order to login again
 * @warning this function is not tested yet
 *
 * @param session_id session_id given by server in response to login
 * @param hostname host to connect to
 * @param port port to connect to
 * @return On success new SSL connection, elsewise NULL
 */
SSL *try_to_reconnect(char *session_id, char *host, int port) {
    int count = 0;
    SSL *new_ssl = NULL;
    cJSON *reconect_request = cJSON_CreateObject();
    cJSON_AddStringToObject(reconect_request, "session_id", session_id);

    while (count < 30 && new_ssl == NULL) {
        count++;
        int socket = setup_client_socket_with_silent_errors(host, port);
        if (socket < 0) {
            sleep(1);
            continue;
        }
        SSL_CTX *ctx = init_ssl_context();
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, socket);
        if (SSL_connect(ssl) != 1) { 
            sleep(1);
            continue;
        }

        new_ssl = ssl;
        send_and_delete_json(new_ssl, &reconect_request);
    }

    return new_ssl;
}

