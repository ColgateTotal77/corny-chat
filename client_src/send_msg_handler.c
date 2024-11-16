#include "GTK.h"

void* send_msg_handler(void* arg, GtkWidget *messages_container) {
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
            add_message(messages_container, message, "10:01", TRUE);
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

