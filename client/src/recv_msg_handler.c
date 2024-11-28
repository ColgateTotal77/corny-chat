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
    bool stop_flag = true;

    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    int time_zone = local_time->__tm_gmtoff;
    struct tm message_time = {0};
    char time_to_send[12];
    struct tm *adjusted_time;

    int number_of_users;
    get_all_clients_userslist(call_data->ssl);
    int counter = 0;

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
            if (command_code_json) {
                if ((command_code_json)->valueint == 11 && cJSON_GetObjectItemCaseSensitive(parsed_json, "success")->valueint > 0) {
                    cJSON *session_id_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "session_id");
                    session_id = (char*)calloc(strlen(session_id_json->valuestring)+ 1, sizeof(char));
                    strncpy(session_id, session_id_json->valuestring, strlen(session_id_json->valuestring));
                    GTK_data->user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                    char* nickname = cJSON_GetObjectItemCaseSensitive(parsed_json, "nickname")->valuestring;
                    GTK_data->username = (char*)calloc(strlen(nickname)+ 1, sizeof(char));
                    strncpy(GTK_data->username, nickname, strlen(nickname));
                    continue;
                }
                else if (command_code_json->valueint == 17 && stop_flag) {                 
                    cJSON *users = cJSON_GetObjectItemCaseSensitive(parsed_json, "users");
                    number_of_users = cJSON_GetObjectItemCaseSensitive(parsed_json, "number_of_users")->valueint;
                    
                    for (int i = 0; i < number_of_users; i++) {
                        cJSON *user = cJSON_GetArrayItem(users, i);
                        
                        int user_id = cJSON_GetObjectItemCaseSensitive(user, "id")->valueint;
                        const char *nickname = cJSON_GetObjectItemCaseSensitive(user, "nickname")->valuestring;
                        bool is_online = cJSON_GetObjectItemCaseSensitive(user, "online")->valueint;
                        scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                        scroll_data->ssl = call_data->ssl;
                        chat_data_t *new_chat = create_chat_data(nickname, user_id, scroll_data);
                        g_hash_table_insert(GTK_data->chat_manager->chats, GINT_TO_POINTER(user_id), new_chat);

                        GtkWidget *new_chat_item = create_chat_item(nickname, user_id, "None", "", is_online, FALSE, GTK_data->chat_manager);

                        GtkWidget *child = gtk_widget_get_first_child(GTK_data->chat_manager->sidebar);
                        gboolean added = FALSE;
                        
                        while (child != NULL) {
                            if (GTK_IS_BUTTON(child) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(child)), "Add new group") == 0) {
                                gtk_box_insert_child_after(GTK_BOX(GTK_data->chat_manager->sidebar), new_chat_item, gtk_widget_get_prev_sibling(child));
                                added = TRUE;
                                break;
                            }
                            child = gtk_widget_get_next_sibling(child);
                        }

                        if (!added) {
                            gtk_box_append(GTK_BOX(GTK_data->chat_manager->sidebar), new_chat_item);
                        }
                        get_num_of_msgs_with_user(call_data->ssl, new_chat->contact_id, new_chat->last_message_id, 15);
                    }
                    stop_flag = false;
                    continue;
                }
                else if(command_code_json->valueint == 24 && counter < number_of_users) {
                    chat_data_t *chat = NULL;
                    int all_mes_qty = cJSON_GetObjectItemCaseSensitive(parsed_json, "all_mes_qty")->valueint;
                    if (all_mes_qty) {
                        cJSON *messages = cJSON_GetObjectItemCaseSensitive(parsed_json, "messages");                                
                        for (int i = 0;  i < all_mes_qty; i++) {
                            cJSON *message_data = cJSON_GetArrayItem(messages, i);
                            
                            char *created_at = cJSON_GetObjectItemCaseSensitive(message_data, "created_at")->valuestring;
                            strptime(created_at, "%Y-%m-%d %H:%M:%S", &message_time);
                            time_t time_value = mktime(&message_time);
                            time_value += time_zone; 
                            adjusted_time = localtime(&time_value);
                            strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                            int owner_id = cJSON_GetObjectItemCaseSensitive(message_data, "owner_id")->valueint;
                            int target_id = cJSON_GetObjectItemCaseSensitive(message_data, "target_id")->valueint;
                            char *message = cJSON_GetObjectItemCaseSensitive(message_data, "message")->valuestring;
                            int msg_id = cJSON_GetObjectItemCaseSensitive(message_data, "msg_id")->valueint;

                            if (owner_id == GTK_data->user_id) {
                                chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(target_id));
                                add_message(chat->messages_container, message, time_to_send, true, GTK_data->chat_manager, create_message_data(msg_id, chat), call_data->ssl);
                                if (i == 0) {
                                    chat->last_message_id = msg_id;
                                }
                                else if(i == all_mes_qty - 1) {
                                    if (local_time->tm_mday > adjusted_time->tm_mday) {
                                        strftime(time_to_send, sizeof(time_to_send), "%Y-%m-%d", adjusted_time);
                                        change_sidebar_chat_info(chat, message, time_to_send);
                                    }
                                    else {
                                        strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                                        change_sidebar_chat_info(chat, message, time_to_send);
                                    }
                                }
                            }
                            else {
                                chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(owner_id));
                                add_message(chat->messages_container, message, time_to_send, false, GTK_data->chat_manager, create_message_data(msg_id, chat), call_data->ssl);
                                if (i == 0) {
                                    chat->last_message_id = msg_id;
                                }
                                else if(i == all_mes_qty - 1) {
                                    if (local_time->tm_mday > adjusted_time->tm_mday) {
                                        strftime(time_to_send, sizeof(time_to_send), "%Y-%m-%d", adjusted_time);
                                        change_sidebar_chat_info(chat, message, time_to_send);
                                    }
                                    else {
                                        strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                                        change_sidebar_chat_info(chat, message, time_to_send);
                                    }
                                }
                            }
                        }   
                    }
                    counter++;
                    continue;
                } 
                    // case 12:
                    //     unreaded_chats_qty = cJSON_GetObjectItemCaseSensitive(parsed_json, "unreaded_chats_qty")->valueint;
                    //     if (unreaded_chats_qty) {
                    //         cJSON *unread_chats_array = cJSON_GetObjectItemCaseSensitive(parsed_json, "unread_chats_data");
                    //         for (int i = 0; i < unreaded_chats_qty; i++) {
                    //             cJSON *unread_chat = cJSON_GetArrayItem(unread_chats_array, i);
                    //             int sender_id = cJSON_GetObjectItemCaseSensitive(unread_chat, "sender")->valueint;
                    //             chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                    //             int all_msgs_qty = cJSON_GetObjectItemCaseSensitive(unread_chat, "all_msgs_qty")->valueint;
                    //             cJSON *last_messages_array = cJSON_GetObjectItemCaseSensitive(unread_chat, "last_messages");                                
                    //             for (int j = 0; j < all_msgs_qty; j++) {
                    //                 cJSON *last_message = cJSON_GetArrayItem(last_messages_array, j);

                    //                 char *created_at = cJSON_GetObjectItemCaseSensitive(last_message, "created_at")->valuestring;
                    //                 strptime(created_at, "%Y-%m-%d %H:%M:%S", &message_time);
                    //                 time_t time_value = mktime(&message_time);
                    //                 time_value += time_zone; 
                    //                 adjusted_time = localtime(&time_value);
                    //                 strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                    //                 int owner_id = cJSON_GetObjectItemCaseSensitive(last_message, "owner_id")->valueint;
                    //                 char *message = cJSON_GetObjectItemCaseSensitive(last_message, "message")->valuestring;
                    //                 add_message(chat->messages_container, message, time_to_send, sender_id != owner_id);
                    //                 if(j == all_msgs_qty - 1) {
                    //                     if (local_time->tm_mday > adjusted_time->tm_mday) {
                    //                         strftime(time_to_send, sizeof(time_to_send), "%Y-%m-%d", adjusted_time);
                    //                         change_sidebar_chat_info(chat, message, time_to_send);
                    //                     }
                    //                     else {
                    //                         strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                    //                         change_sidebar_chat_info(chat, message, time_to_send);
                    //                     }
                    //                 }
                    //             }
                    //         }   
                    //     }
                    //     stop_flag = false;
                    //     break;
                if(!stop_flag && counter == number_of_users){
                    switch (command_code_json->valueint)
                    {
                    cJSON *users;
                    int all_mes_qty;
                    chat_data_t *chat = NULL;
                    case 24:
                        all_mes_qty = cJSON_GetObjectItemCaseSensitive(parsed_json, "all_mes_qty")->valueint;
                        if (all_mes_qty) {
                            cJSON *messages = cJSON_GetObjectItemCaseSensitive(parsed_json, "messages");                                
                            for (int i = all_mes_qty - 1;  i >= 0; i--) {
                                cJSON *message_data = cJSON_GetArrayItem(messages, i);
                                
                                char *created_at = cJSON_GetObjectItemCaseSensitive(message_data, "created_at")->valuestring;
                                strptime(created_at, "%Y-%m-%d %H:%M:%S", &message_time);
                                time_t time_value = mktime(&message_time);
                                time_value += time_zone; 
                                adjusted_time = localtime(&time_value);
                                strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                                int owner_id = cJSON_GetObjectItemCaseSensitive(message_data, "owner_id")->valueint;
                                int target_id = cJSON_GetObjectItemCaseSensitive(message_data, "target_id")->valueint;
                                char *message = cJSON_GetObjectItemCaseSensitive(message_data, "message")->valuestring;
                                int msg_id = cJSON_GetObjectItemCaseSensitive(message_data, "msg_id")->valueint;

                                if (i == 0) {
                                    if(owner_id == GTK_data->user_id) {
                                        chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(target_id));
                                        chat->last_message_id = msg_id;
                                    }
                                    else {
                                        chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(owner_id));
                                        chat->last_message_id = msg_id;
                                    }
                                }

                                if (owner_id == GTK_data->user_id) {
                                    chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(target_id));
                                    add_message_to_top(chat->messages_container, message, time_to_send, true, chat->adjustment, GTK_data->chat_manager, create_message_data(msg_id, chat), call_data->ssl);
                                }
                                else {
                                    chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(owner_id));
                                    add_message_to_top(chat->messages_container, message, time_to_send, false, chat->adjustment, GTK_data->chat_manager, create_message_data(msg_id, chat), call_data->ssl);
                                }
                            }   
                        } 
                        break;
                    case 17:
                        users = cJSON_GetObjectItemCaseSensitive(parsed_json, "users");
                        number_of_users = cJSON_GetObjectItemCaseSensitive(parsed_json, "number_of_users")->valueint;
                        
                        // Clear existing login list first
                        if (GTK_data->profile_data && GTK_data->profile_data->login_list) {
                            GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->login_list));
                            while (child) {
                                GtkWidget *next = gtk_widget_get_next_sibling(child);
                                gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->login_list), child);
                                child = next;
                            }
                        }

                        for (int i = 0; i < number_of_users; i++) {
                            cJSON *user = cJSON_GetArrayItem(users, i);
                            
                            int user_id = cJSON_GetObjectItemCaseSensitive(user, "id")->valueint;
                            const char *nickname = cJSON_GetObjectItemCaseSensitive(user, "nickname")->valuestring;
                            //bool is_online = cJSON_GetObjectItemCaseSensitive(user, "online")->valueint;

                            // Skip FirstAdmin and inactive users
                            if (user_id == 1 || g_strcmp0(nickname, "FirstAdmin") == 0) {
                                continue;
                            }

                            cJSON *active = cJSON_GetObjectItemCaseSensitive(user, "active");
                            if (!cJSON_IsBool(active) || !cJSON_IsTrue(active)) {
                                continue;
                            }

                            // Update login list
                            if (GTK_data->profile_data && GTK_data->profile_data->login_list) {
                                GtkWidget *row = gtk_list_box_row_new();
                                GtkWidget *label = gtk_label_new(nickname);
                                gtk_widget_set_halign(label, GTK_ALIGN_START);
                                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->login_list), row);
                            }
                        }
                        // Ensure login list is visible
                        if (GTK_data->profile_data && GTK_data->profile_data->login_list) {
                            gtk_widget_set_visible(GTK_WIDGET(GTK_data->profile_data->login_list), TRUE);
                        }
                        break;
                    case 16:{
                        cJSON *json = cJSON_Parse(message);
                        if (!json) {
                            fprintf(stderr, "Error: Failed to parse JSON response\n");
                            break;
                        }

                        cJSON *success = cJSON_GetObjectItemCaseSensitive(json, "success");
                        if (cJSON_IsBool(success)) {
                            if (GTK_data && GTK_data->profile_data) {
                                if (success->valueint) {
                                    // Success case
                                    display_ui_message(GTK_data, "Account successfully created!", true);
                                } else {
                                    // Error case
                                    cJSON *err_msg = cJSON_GetObjectItemCaseSensitive(json, "err_msg");
                                    const char *error_text = (err_msg && cJSON_IsString(err_msg)) ? 
                                                        err_msg->valuestring : "Account creation failed";
                                    display_ui_message(GTK_data, error_text, false);
                                }
                            }
                        }

                        cJSON_Delete(json);
                        break;
                    }
                    case 1:
                        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(parsed_json, "success"))) {
                            int contact_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "contact_id")->valueint;
                            int message_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                            char *created_at = cJSON_GetObjectItemCaseSensitive(parsed_json, "time_reached_server")->valuestring;
                            char *message = cJSON_GetObjectItemCaseSensitive(parsed_json, "message")->valuestring;
                            chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(contact_id));

                            strptime(created_at, "%Y-%m-%d %H:%M:%S", &message_time);
                            time_t time_value = mktime(&message_time);
                            time_value += time_zone; 
                            adjusted_time = localtime(&time_value);
                            strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                            
                            add_message(chat->messages_container, message, time_to_send, true, GTK_data->chat_manager, create_message_data(message_id, chat), call_data->ssl);
                        }
                        break;      
                    default:

                        break;
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
                        char *msg = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(parsed_json, "message"));
                        char time_str[6];
                        strftime(time_str, sizeof(time_str), "%H:%M", local_time);
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        add_message(chat->messages_container, msg, time_str, false, GTK_data->chat_manager, create_message_data(msg_id, chat), call_data->ssl);
                        change_sidebar_chat_info(chat, msg, time_str);
                    }
                    break;
                }
                case 53: {
                    int user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                    chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(user_id));
                    if(chat != NULL) {
                        change_status_sidebar_chat(chat, true);
                    }
                    break;
                }
                case 54: {
                    int user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                    chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(user_id));
                    if(chat != NULL) {
                        change_status_sidebar_chat(chat, false);
                    }
                    break;
                }
                case 57: {
                    int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                    int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                    char *new_message = cJSON_GetObjectItemCaseSensitive(parsed_json, "new_message")->valuestring;
                    chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                    change_message_from_others(chat, msg_id, new_message);
                    break;
                }
                case 58: {
                    int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                    int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                    chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                    delete_message_from_others(chat, msg_id);
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

