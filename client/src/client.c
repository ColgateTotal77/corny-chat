#include "GTK.h"

bool stop_flag;

SSL_CTX *init_ssl_context(void) {
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

void catch_ctrl_c_and_exit(int sig) {
    (void)sig;
    stop_flag = true;
    printf("Press ENTER to end process\n");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    // Initialize GTK before creating threads
    gtk_init();

    if (argc != 3) {
        fprintf(stderr, "usage: %s <hostname> <port_number>\n", argv[0]);
        return EXIT_FAILURE;
    }


    //cJSON *json_name_and_password = get_name_password(); //закоментить
    
    int port = atoi(argv[2]);

    int sock = setup_client_socket(argv[1], port);

    SSL_CTX *ctx = init_ssl_context();
    SSL *ssl = SSL_new(ctx);  
    SSL_set_fd(ssl, sock);    
    if (SSL_connect(ssl) != 1) { 
        fprintf(stderr, "SSL connect failed\n");
        ERR_print_errors_fp(stderr);
        close(sock);
        exit(1);
    }

    struct sigaction psa = {.sa_handler = catch_ctrl_c_and_exit};
    sigaction(SIGINT, &psa, NULL);
    stop_flag = false;
    
    call_data_t *call_data = (call_data_t *)malloc(sizeof(call_data_t));
    call_data->sockfd = sock; 
    call_data->ssl = ssl;
    call_data->stop_flag = &stop_flag;
    
    call_data->host = argv[1];
    call_data->port = atoi(argv[2]);
    // strcpy(call_data->name, name_json->valuestring);
    // cJSON_Delete(json_name_and_password);

    start_login(call_data);
    // GTK_start(call_data);

    while (1) {
        if (stop_flag) {
            //pthread_join(send_msg_thread, NULL);
            printf("\nBye\n");
            // SSL_shutdown(ssl);//Закриття SSL з'єднання
            SSL_free(ssl);
            break;
        }
        sleep(1);
    }

    free(call_data);
    close(sock);

    SSL_CTX_free(ctx);
    return EXIT_SUCCESS;
}
