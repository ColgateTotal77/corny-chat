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
