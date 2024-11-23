#include "GTK.h"

static char* get_login_for_new_user(void) {
    printf("Login must be less than 30 and more than 1 characters.\n");
    printf("Please enter login: ");
    char login[32];
    fgets(login, 32, stdin);
    str_del_newline(login, strlen(login));
    int login_length = strlen(login);

	if (login_length > 30 || login_length < 2) {
		printf("Invalid input. Login must be less than 30 and more than 1 characters.\n");
		return NULL;
	}

    char *login_buffer = (char*)malloc(login_length * sizeof(char) + 1);
    login_buffer[login_length] = '\0';
    strncpy(login_buffer, login, login_length);

    return login_buffer;
}

static char* get_password_for_new_user(void) {
    printf("Password must be less than 20 and more than 8 characters. Also, only keyboard symbols are allowed, without spaces.\n");
    printf("Enter password: ");
    char password[22];
    fgets(password, 22, stdin);
    str_del_newline(password, strlen(password));
    int password_length = strlen(password);
    if (password_length > 20 || password_length < 8) {
		printf("Invalid input. Password is incorrest!.\n");
		return NULL;
	}

    char *password_buffer = (char*)malloc(password_length * sizeof(char) + 1);
    password_buffer[password_length] = '\0';
    strncpy(password_buffer, password, password_length);

    return password_buffer;
}

void* send_msg_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;

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
                      "GET_ALL_TALKS 12\n"
                      "UPDATE_NICKNAME 14\n"
                      "CHANGE_PASSWORD 15\n"
                      "CREATE_USER 16\n"
                      "GET_ALL_CLIENTS_USERSLIST 17\n"
                      "MARK_CHAT_MSGS_AS_READED 18\n"
                      "GET_MY_CLIENTS_USERSLIST 19\n"
                      "DELETE_USER_FROM_GROUP 20\n"
                      "ADD_MANY_USERS_TO_GROUP 21\n"
                      "DELETE_GROUP 22\n"
                      "ADMIN_CHANGE_PASSWORD 23\n"
                      "GET_NUM_OF_MSGS_BETWEEN 24\n";
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

        char *login;
        char *password;
        int *users_array;
        int users_count;

        int user2;
        int before;
        int quantity;


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
            send_to_chat(call_data->ssl, chat_id, message); 
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
            send_to_user(call_data->ssl, user_id, message);
            // add_message(messages_container, message, "10:01", TRUE);
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
            printf("Enter new contact id: ");
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
        case UPDATE_NICKNAME:
            bzero(message, BUF_SIZE);
            printf("Enter new nickname: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            update_my_nickname(call_data->ssl, message);
            break;
        case CHANGE_PASSWORD:
            bzero(message, BUF_SIZE);
            printf("Enter old password: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            char old_password[20];
            strcpy(old_password, message);

            bzero(message, BUF_SIZE);
            printf("Enter new password: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);

            change_password(call_data->ssl, old_password, message);
            break;
        case CREATE_USER:
            login = get_login_for_new_user();
            if (login == NULL) {
                break;
            }
            password = get_password_for_new_user();
            if (password == NULL) {
                free(login);
                break;
            }
            bool is_admin = true;
            create_new_user(call_data->ssl, login, password, is_admin);
            free(login);
            free(password);
            break;
        case GET_ALL_CLIENTS_USERSLIST:
            get_all_clients_userslist(call_data->ssl);
            break;
        case MARK_CHAT_MSGS_AS_READED:
            bzero(message, BUF_SIZE);
            printf("Enter sender id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            contact_id = atoi(message);

            mark_chat_msgs_as_readed(call_data->ssl, contact_id);
            break;
        case GET_MY_CLIENTS_USERSLIST:
            get_my_clients_userslist(call_data->ssl);
            break;
        case DELETE_USER_FROM_GROUP:
            bzero(message, BUF_SIZE);
            printf("Enter user id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            contact_id = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter chat id:");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);
            delete_user_from_group(call_data->ssl, contact_id, chat_id);
            break;
        case ADD_MANY_USERS_TO_GROUP:
            bzero(message, BUF_SIZE);
            printf("Enter chat id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);
            users_array = NULL;
            users_array = (int*)malloc(0);
            users_count = 0;

            while (true) {
                bzero(message, BUF_SIZE);
                printf("Enter user id(or nothing to break): ");
                fflush(stdout);
                fgets(message, BUF_SIZE, stdin);
                str_del_newline(message, BUF_SIZE);
                if (strcmp(message, "") == 0) {
                    break;
                }
                contact_id = atoi(message);
                append_to_intarr(&users_array, &users_count, contact_id);
            }

            add_many_users_to_group(call_data->ssl, users_array, users_count, chat_id);
            free(users_array);
            users_array = NULL;
            break;
        case DELETE_GROUP:
            bzero(message, BUF_SIZE);
            printf("Enter chat id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            chat_id = atoi(message);

            delete_group(call_data->ssl, chat_id); 
            break;
        case ADMIN_CHANGE_PASSWORD:
            bzero(message, BUF_SIZE);
            printf("Enter user id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            user_id = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter new password: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            admin_change_password(call_data->ssl, user_id, message); 
            break;
        case GET_NUM_OF_MSGS_BETWEEN:
            bzero(message, BUF_SIZE);
            printf("Enter user 2: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            user2 = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter before id: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            before = atoi(message);

            bzero(message, BUF_SIZE);
            printf("Enter quantity: ");
            fflush(stdout);
            fgets(message, BUF_SIZE, stdin);
            str_del_newline(message, BUF_SIZE);
            quantity = atoi(message);
            get_num_of_msgs_with_user(call_data->ssl, user2, before, quantity);
            break;
        default:
            printf("Wrong command code\n");
            fflush(stdout);
            break;
        }
        sleep(1);
        bzero(message, BUF_SIZE);
       // bzero(buffer, BUF_SIZE + 32);
    }

    *(call_data->stop_flag) = true;
    shutdown(call_data->sockfd, SHUT_RDWR);
    
    pthread_detach(pthread_self());

    return NULL;
}
