#include "client.h"
#include "commands.h"
#include <gtk/gtk.h>
#include "GTK.h"

bool stop_flag;

void str_overwrite_stdout(void) {
    printf("%s", "> ");
    fflush(stdout);
}

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

//void* handle_user_input(void* arg) {
//    call_data_t* call_data = (call_data_t*)arg;
//    char message[BUF_SIZE];
//
//    while (1) {
//
//    }
//
//    *(call_data->stop_flag) = true;
//    
//    return NULL;
//}

void* send_msg_handler(void* arg) {
    call_data_t* call_data = (call_data_t*)arg;
    char message[BUF_SIZE];
    //char buffer[BUF_SIZE + 32];
    int chat_id;
    int contact_id;
    int user_id;

    char *help_info = "SEND_TO_CHAT 0\n"
                      "SEND_TO_USER 1\n"
                      "CREATE_CHAT 2\n"
                      "SEE_ALL_CHATS 3\n"
                      "SEE_ALL_USERS 4\n"
                      "ADD_CONTACT 5\n"
                      "SEE_MY_CONTACTS 6\n"
                      "JOIN_CHAT 7\n"
                      "ADD_CONTACT_TO_CHAT 8\n"
                      "EXIT 9\n"
                      "GET_MY_CONTACTS 10\n"
                      "GET_ALL_TALKS 12\n";
    printf("%s", help_info);
    printf("Enter command code and follow the instructions. This is for test\n");
    fflush(stdout);

    while (!*(call_data->stop_flag)) {
        //str_overwrite_stdout();
        fgets(message, BUF_SIZE, stdin);
        str_del_newline(message, BUF_SIZE);
        char* remains;
        int command = strtol(message, &remains, 0); 
        if (strlen(message) == strlen(remains)) {
            printf("Programs needs a number for command not an string!"); 
            fflush(stdout);
            bzero(message, BUF_SIZE);
            continue;
        }
        else  if (strlen(remains) != 0) {
            printf("Not valid command -- extra stuff = %s", remains );
            fflush(stdout);
            bzero(message, BUF_SIZE);
            continue;
        }

        switch (command) {
        case SEND_TO_CHAT:
            bzero(message, BUF_SIZE);
            printf("Enter chat id of chat to send: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter Message to send to chat: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            send_to_chat(call_data->ssl, chat_id, message);  // Отправка через стандартный сокет
            break;
        case SEND_TO_USER:
            bzero(message, BUF_SIZE);
            printf("Enter user id to send msg to: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            user_id = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter Message to send to user: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            send_to_user(call_data->ssl, user_id, message);  // Отправка через стандартный сокет
            break;
        case CREATE_CHAT:
            bzero(message, BUF_SIZE);
            printf("Enter new chat name: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            create_chat(call_data->ssl, message);
            break;
        case SEE_ALL_CHATS:
            see_all_chats(call_data->ssl);
            break;
        case SEE_ALL_USERS:
            see_all_users(call_data->ssl);
            break;
        case ADD_CONTACT:
            bzero(message, BUF_SIZE);
            printf("Enter new contact id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            contact_id = atoi(message);

            add_contact(call_data->ssl, contact_id);
            break;
        case SEE_MY_CONTACTS:
            see_my_contacts(call_data->ssl);
            break;
        case JOIN_CHAT:
            bzero(message, BUF_SIZE);
            printf("Enter chat id to join: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);

            join_chat(call_data->ssl, chat_id);
            break;
        case ADD_CONTACT_TO_CHAT:
            bzero(message, BUF_SIZE);
            printf("Enter contact id to add to chat:");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            contact_id = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter chat id to add contact to:");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);
            add_contact_to_chat(call_data->ssl, contact_id, chat_id);
            break;
        case EXIT:
            send_exit_command(call_data->ssl);
            *(call_data->stop_flag) = true;
            break;
        case GET_MY_CONTACTS:
            get_my_contacts(call_data->ssl);
            break;
        case GET_ALL_TALKS:
            get_all_talks(call_data->ssl);
            break;
        default:
            printf("Wrong command code\n");
            fflush(stdout);
            break;
        }
        
        bzero(message, BUF_SIZE);
        sleep(1);
       // bzero(buffer, BUF_SIZE + 32);
    }

    *(call_data->stop_flag) = true;
    shutdown(call_data->sockfd, SHUT_RDWR);
    
    pthread_detach(pthread_self());

    return NULL;
}

void* recv_msg_handler(void* arg) {
    call_data_t* call_data = (call_data_t*)arg;
    char message[1024];

    while (!*(call_data->stop_flag)) {
        int bytes_received = SSL_read(call_data->ssl, message, 1024);  // Используем SSL для чтения

        if (bytes_received > 0) {
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

void catch_ctrl_c_and_exit(int sig) {
    (void)sig;
    stop_flag = true;
    printf("Press ENTER to end process\n");
    fflush(stdout);
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <hostname> <port_number>\n", argv[0]);
        return EXIT_FAILURE;
    }


    //cJSON *json_name_and_password = get_name_password();
    
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

    // char *str_json_name_password = cJSON_Print(json_name_and_password);
    // cJSON_Minify(str_json_name_password);

    // SSL_write(ssl, str_json_name_password, strlen(str_json_name_password));

    // free(str_json_name_password);

    // cJSON *name_json = cJSON_GetObjectItemCaseSensitive(json_name_and_password, "name");
    
    call_data_t *call_data = (call_data_t *)malloc(sizeof(call_data_t));
    call_data->sockfd = sock; 
    call_data->ssl = ssl;
    call_data->stop_flag = &stop_flag;
    // strcpy(call_data->name, name_json->valuestring);

    // cJSON_Delete(json_name_and_password);
    
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, &send_msg_handler, (void*)call_data) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, (void*)call_data) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }


    start_login(ssl);
    //GTK_start(ssl);


    while (1) {
        if (stop_flag) {
            pthread_join(send_msg_thread, NULL);
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

