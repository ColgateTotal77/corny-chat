#include "GTK.h"

void str_overwrite_stdout(void) {
    printf("%s", "> ");
    fflush(stdout);
}

void* recv_msg_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;

    cJSON *parsed_json;

    char *session_id = NULL;
    while (!*(call_data->stop_flag)) {
        //bzero(message, 1024);
        char* message = NULL;
        int bytes_received = recieve_next_response(call_data->ssl, &message); 

        if (bytes_received > 0) {
            printf("message = %s\n",message);
            parsed_json = cJSON_Parse(message);
            if (!parsed_json) {
                continue;
            }

            cJSON *command_code_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "command_code");
            if(command_code_json) {
                if ((command_code_json)->valueint == 11) {
                    cJSON *session_id_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "session_id");
                    session_id = (char*)calloc(strlen(session_id_json->valuestring)+ 1, sizeof(char));
                    strncpy(session_id, session_id_json->valuestring, strlen(session_id_json->valuestring));
                    continue;
                }

                if (command_code_json->valueint == 17) {
                    cJSON *users = cJSON_GetObjectItemCaseSensitive(parsed_json, "users");

                    int number_of_users = cJSON_GetObjectItemCaseSensitive(parsed_json, "number_of_users")->valueint;
                    for (int i = 0; i < number_of_users; i++) {
                        cJSON *user = cJSON_GetArrayItem(users, i);
                        
                        int user_id = cJSON_GetObjectItemCaseSensitive(user, "id")->valueint;
                        const char *nickname = cJSON_GetObjectItemCaseSensitive(user, "nickname")->valuestring;

                        chat_data_t *new_chat = create_chat_data(nickname, user_id);
                        g_hash_table_insert(GTK_data->chat_manager->chats, GINT_TO_POINTER(user_id), new_chat);

                        GtkWidget *new_chat_item = create_chat_item(nickname, user_id, "None", "12:00", TRUE, FALSE, GTK_data->chat_manager);

                        GtkWidget *child = gtk_widget_get_first_child(GTK_data->sidebar);
                        gboolean added = FALSE;
                        
                        while (child != NULL) {
                            if (GTK_IS_BUTTON(child) &&
                                g_strcmp0(gtk_button_get_label(GTK_BUTTON(child)), "Add new group") == 0) {
                                gtk_box_insert_child_after(GTK_BOX(GTK_data->sidebar), new_chat_item, gtk_widget_get_prev_sibling(child));
                                added = TRUE;
                                break;
                            }
                            child = gtk_widget_get_next_sibling(child);
                        }

                        if (!added) {
                            gtk_box_append(GTK_BOX(GTK_data->sidebar), new_chat_item);
                        }
                    }
                    continue;
                }
            }
            cJSON *event_code_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "event_code");
            if (!event_code_json || !cJSON_IsNumber(event_code_json)) {
                cJSON_Delete(parsed_json);
                continue;
            }
            int event_code = event_code_json->valueint;
            switch (event_code) {
                case 51: {
                    int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                    chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                    if (chat) {
                        const char *msg = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(parsed_json, "message"));
                        time_t now = time(NULL);
                        struct tm *t = localtime(&now);
                        char time_str[6];
                        strftime(time_str, sizeof(time_str), "%H:%M", t);
                        add_message(chat->messages_container, msg, time_str, false);
                    }
                    break;
                }
            }
            cJSON_Delete(parsed_json);
            free(message);
        } 
        else if (bytes_received == 0) {
            if (*(call_data->stop_flag)) {
                printf("SSL = NULL\nServer disconnected!\n");
                free(session_id);
                session_id = NULL;
                break;
            }
            SSL* new_ssl = try_to_reconnect(session_id, call_data->host, call_data->port);

            if (new_ssl == NULL) {
                printf("SSL = NULL\nServer disconnected!\n");
                free(session_id);
                session_id = NULL;
                *(call_data->stop_flag) = true;
                break;
            }
            printf("\nConnection recover!\n");
            SSL_free(call_data->ssl);
            call_data->ssl = new_ssl;
            continue;
        } 
        else {
            int err = SSL_get_error(call_data->ssl, bytes_received);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                break;
            }
        }
        //memset(message, 0, sizeof(message));
    }
    free(session_id);
    pthread_detach(pthread_self());
    return NULL;
}

