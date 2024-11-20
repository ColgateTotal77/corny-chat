#include "GTK.h"

bool stop_flag;

// void str_overwrite_stdout(void) {
//     printf("%s", "> ");
//     fflush(stdout);
// }

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

// static char* get_login_for_new_user(void) {
//     printf("Login must be less than 30 and more than 1 characters.\n");
//     printf("Please enter login: ");
//     char login[32];
//     fgets(login, 32, stdin);
//     str_del_newline(login, strlen(login));
//     int login_length = strlen(login);

// 	if (login_length > 30 || login_length < 2) {
// 		printf("Invalid input. Login must be less than 30 and more than 1 characters.\n");
// 		return NULL;
// 	}

//     char *login_buffer = (char*)malloc(login_length * sizeof(char) + 1);
//     login_buffer[login_length] = '\0';
//     strncpy(login_buffer, login, login_length);

//     return login_buffer;
// }

// static char* get_password_for_new_user(void) {
//     printf("Password must be less than 20 and more than 8 characters. Also, only keyboard symbols are allowed, without spaces.\n");
//     printf("Enter password: ");
//     char password[22];
//     fgets(password, 22, stdin);
//     str_del_newline(password, strlen(password));
//     int password_length = strlen(password);
//     if (password_length > 20 || password_length < 8) {
// 		printf("Invalid input. Password is incorrest!.\n");
// 		return NULL;
// 	}

//     char *password_buffer = (char*)malloc(password_length * sizeof(char) + 1);
//     password_buffer[password_length] = '\0';
//     strncpy(password_buffer, password, password_length);

//     return password_buffer;
// }


// void* send_msg_handler(void* arg) {
//     call_data_t* call_data = (call_data_t*)arg;
//     char message[BUF_SIZE];
//     //char buffer[BUF_SIZE + 32];
//     int chat_id;
//     int contact_id;
//     int user_id;

//     char *help_info = "SEND_TO_CHAT 0\n"
//                       "SEND_TO_USER 1\n"
//                       "CREATE_CHAT 2\n"
//                       "SEE_ALL_CHATS 3\n"
//                       "SEE_ALL_USERS 4\n"
//                       "ADD_CONTACT 5\n"
//                       "SEE_MY_CONTACTS 6\n"
//                       "JOIN_CHAT 7\n"
//                       "ADD_CONTACT_TO_CHAT 8\n"
//                       "EXIT 9\n"
//                       "GET_MY_CONTACTS 10\n"
//                       "GET_ALL_TALKS 12\n"
//                       "UPDATE_NICKNAME 14\n"
//                       "CHANGE_PASSWORD 15\n"
//                       "CREATE_USER 16\n";
//     printf("%s", help_info);
//     printf("Enter command code and follow the instructions. This is for test\n");
//     fflush(stdout);

//     while (!*(call_data->stop_flag)) {
//         //str_overwrite_stdout();
//         fgets(message, BUF_SIZE, stdin);
//         str_del_newline(message, BUF_SIZE);
//         char* remains;
//         int command = strtol(message, &remains, 0); 
//         if (strlen(message) == strlen(remains)) {
//             printf("Programs needs a number for command not an string!"); 
//             fflush(stdout);
//             bzero(message, BUF_SIZE);
//             continue;
//         }
//         else  if (strlen(remains) != 0) {
//             printf("Not valid command -- extra stuff = %s", remains );
//             fflush(stdout);
//             bzero(message, BUF_SIZE);
//             continue;
//         }

//         char *login;
//         char *password;

//         switch (command) {
//         case SEND_TO_CHAT:
//             bzero(message, BUF_SIZE);
//             printf("Enter chat id of chat to send: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             chat_id = atoi(message);

//             bzero(message, BUF_SIZE);
//             printf("Enter Message to send to chat: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             //send_to_chat(call_data->ssl, chat_id, message); 
//             break;
//         case SEND_TO_USER:
//             bzero(message, BUF_SIZE);
//             printf("Enter user id to send msg to: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             user_id = atoi(message);

//             bzero(message, BUF_SIZE);
//             printf("Enter Message to send to user: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             send_to_user(call_data->ssl, user_id, message);
//             //add_message(messages_container, message, "10:01", TRUE);
//             break;
//         case CREATE_CHAT:
//             bzero(message, BUF_SIZE);
//             printf("Enter new chat name: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             create_chat(call_data->ssl, message);
//             break;
//         case SEE_ALL_CHATS:
//             see_all_chats(call_data->ssl);
//             break;
//         case SEE_ALL_USERS:
//             see_all_users(call_data->ssl);
//             break;
//         case ADD_CONTACT:
//             bzero(message, BUF_SIZE);
//             printf("Enter new contact id: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             contact_id = atoi(message);

//             add_contact(call_data->ssl, contact_id);
//             break;
//         case SEE_MY_CONTACTS:
//             see_my_contacts(call_data->ssl);
//             break;
//         case JOIN_CHAT:
//             bzero(message, BUF_SIZE);
//             printf("Enter chat id to join: ");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             chat_id = atoi(message);

//             join_chat(call_data->ssl, chat_id);
//             break;
//         case ADD_CONTACT_TO_CHAT:
//             bzero(message, BUF_SIZE);
//             printf("Enter contact id to add to chat:");
//             fflush(stdout);
//             fgets(message, BUF_SIZE, stdin);
//             str_del_newline(message, BUF_SIZE);
//             contact_id = atoi(message);

        //     bzero(message, BUF_SIZE);
        //     printf("Enter chat id to add contact to:");
        //     fflush(stdout);
        //     fgets(message, BUF_SIZE, stdin);
        //     str_del_newline(message, BUF_SIZE);
        //     chat_id = atoi(message);
        //     add_contact_to_chat(call_data->ssl, contact_id, chat_id);
        //     break;
        // case EXIT:
        //     send_exit_command(call_data->ssl);
        //     *(call_data->stop_flag) = true;
        //     break;
        // case GET_MY_CONTACTS:
        //     get_my_contacts(call_data->ssl);
        //     break;
        // case GET_ALL_TALKS:
        //     get_all_talks(call_data->ssl);
        //     break;
        // case UPDATE_NICKNAME:
        //     bzero(message, BUF_SIZE);
        //     printf("Enter new nickname: ");
        //     fflush(stdout);
        //     fgets(message, BUF_SIZE, stdin);
        //     str_del_newline(message, BUF_SIZE);
        //     update_my_nickname(call_data->ssl, message);
        //     break;
        // case CHANGE_PASSWORD:
        //     bzero(message, BUF_SIZE);
        //     printf("Enter old password: ");
        //     fflush(stdout);
        //     fgets(message, BUF_SIZE, stdin);
        //     str_del_newline(message, BUF_SIZE);
        //     char old_password[20];
        //     strcpy(old_password, message);

        //     bzero(message, BUF_SIZE);
        //     printf("Enter new password: ");
        //     fflush(stdout);
        //     fgets(message, BUF_SIZE, stdin);
        //     str_del_newline(message, BUF_SIZE);

        //     change_password(call_data->ssl, old_password, message);
        //     break;
        // case CREATE_USER:
        //     login = get_login_for_new_user();
        //     if (login == NULL) {
        //         break;
        //     }
        //     password = get_password_for_new_user();
        //     if (password == NULL) {
        //         free(login);
        //         break;
        //     }

        //     create_new_user(call_data->ssl, login, password);
        //     free(login);
        //     free(password);
        //     break;
        // default:
        //     printf("Wrong command code\n");
        //     fflush(stdout);
        //     break;
        // }
        
//         bzero(message, BUF_SIZE);
//         sleep(1);
//        // bzero(buffer, BUF_SIZE + 32);
//     }

//     *(call_data->stop_flag) = true;
//     shutdown(call_data->sockfd, SHUT_RDWR);
    
//     pthread_detach(pthread_self());

//     return NULL;
// }

// void* recv_msg_handler(void* arg) {
//     call_data_t* call_data = (call_data_t*)arg;
//     char message[1024];

//     while (!*(call_data->stop_flag)) {
//         int bytes_received = SSL_read(call_data->ssl, message, 1024);  // Используем SSL для чтения

//         if (bytes_received > 0) {
//             //add_message(messages_container, message, "10:00", false);
//             printf("%s", message);
//             str_overwrite_stdout();
//         }
//         else if (bytes_received == 0) {
//             printf("\nServer disconnected\n");
//             //*(call_data->stop_flag) = true;
//             fflush(stdout);
//             break;
//         }
        
//         else {
//             int err = SSL_get_error(call_data->ssl, bytes_received);
//             if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
//                 continue;
//             }
//             fprintf(stderr, "SSL_read failed with error: %d\n", err);
//             //*(call_data->stop_flag) = true;
//             break;
//         }

//         memset(message, 0, sizeof(message));
//     }

//     pthread_detach(pthread_self());
    
//     return NULL;
// }

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

    // char *str_json_name_password = cJSON_Print(json_name_and_password);//закоментить
    // cJSON_Minify(str_json_name_password);//закоментить

    // SSL_write(ssl, str_json_name_password, strlen(str_json_name_password)); //закоментить

    //free(str_json_name_password); //закоментить

    //cJSON *name_json = cJSON_GetObjectItemCaseSensitive(json_name_and_password, "name");
    
    call_data_t *call_data = (call_data_t *)malloc(sizeof(call_data_t));
    call_data->sockfd = sock; 
    call_data->ssl = ssl;
    call_data->stop_flag = &stop_flag;
    call_data->host = argv[1];
    call_data->port = atoi(argv[2]);
    // strcpy(call_data->name, name_json->valuestring);
    // cJSON_Delete(json_name_and_password);

    // Start login in the main thread
    start_login(ssl);
    GTK_start(call_data);
    //profile_start(ssl);
    // pthread_t send_msg_thread;
    // if (pthread_create(&send_msg_thread, NULL, &send_msg_handler, (void*)call_data) != 0) {
    //     printf("ERROR: pthread\n");
    //     return EXIT_FAILURE;
    // }

    // pthread_t recv_msg_thread;
    // if (pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, (void*)call_data) != 0) {
    //     printf("ERROR: pthread\n");
    //     return EXIT_FAILURE;
    // }

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

