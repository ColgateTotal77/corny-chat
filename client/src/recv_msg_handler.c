#include "GTK.h"

void str_overwrite_stdout(void) {
    printf("%s", "> ");
    fflush(stdout);
}

void create_user_in_sidebar(int user_id, char* nickname, bool is_online, GTK_data_t *GTK_data, chat_data_t *new_chat) {
    g_hash_table_insert(GTK_data->chat_manager->chats, GINT_TO_POINTER(user_id), new_chat);
    GtkWidget *new_chat_item = create_chat_item(nickname, user_id, "None", "", is_online, FALSE, GTK_data);

    gtk_box_append(GTK_BOX(GTK_data->chat_manager->sidebar), new_chat_item);
}

void create_group_in_sidebar(const int chat_id, const char* chat_name, GTK_data_t *GTK_data, chat_data_t *new_chat) {
    g_hash_table_insert(GTK_data->group_manager->chats, GINT_TO_POINTER(chat_id), new_chat);
    GtkWidget *new_group_item = create_chat_item(chat_name, chat_id, "None", "", false, true, GTK_data);

    gtk_box_append(GTK_BOX(GTK_data->group_manager->sidebar), new_group_item);
}

void* recv_msg_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;

    cJSON *parsed_json;
    char *session_id = GTK_data->session_id;
    bool stop_flag = true;

    time_t now = time(NULL);
    struct tm *local_time = malloc(sizeof(struct tm));
    memcpy(local_time, localtime(&now), sizeof(struct tm));
    int time_zone = local_time->__tm_gmtoff;
    struct tm message_time = {0};
    char time_to_send[12];
    struct tm *adjusted_time;
    printf("local_time: %d-%d-%d %d:%d:%d\n", local_time->tm_year, local_time->tm_mon, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
    int number_of_users;
    int groups_count = 0;
    get_all_clients_userslist(call_data->ssl);
    
    int counter = 0;
    int group_counter = 0;
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
                if (command_code_json->valueint == 17 && stop_flag) {                 
                    cJSON *users = cJSON_GetObjectItemCaseSensitive(parsed_json, "users");
                    number_of_users = cJSON_GetObjectItemCaseSensitive(parsed_json, "number_of_users")->valueint;
                    
                    for (int i = 0; i < number_of_users; i++) {
                        cJSON *user = cJSON_GetArrayItem(users, i);
                        
                        int user_id = cJSON_GetObjectItemCaseSensitive(user, "id")->valueint;
                        char *nickname = cJSON_GetObjectItemCaseSensitive(user, "nickname")->valuestring;
                        bool is_online = cJSON_GetObjectItemCaseSensitive(user, "online")->valueint;
                        int unread_messages = cJSON_GetObjectItemCaseSensitive(user, "unread_messages")->valueint;
                        bool is_active = cJSON_GetObjectItemCaseSensitive(user, "active")->valueint;

                        scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                        scroll_data->ssl = call_data->ssl;
                        chat_data_t *new_chat = create_chat_data(nickname, user_id, scroll_data);

                        new_chat->is_active = is_active;
                        create_user_in_sidebar(user_id, nickname, is_online, GTK_data, new_chat);
                        if (!is_active) gtk_image_set_from_file(GTK_IMAGE(new_chat->avatar_circle), "src/chat_visual/images/RIP.svg");
                        
                        if (unread_messages > 0 && unread_messages < 99) {
                            gtk_label_set_text(GTK_LABEL(new_chat->number_of_unread_messages), mx_itoa(unread_messages));
                            gtk_widget_set_visible(new_chat->number_of_unread_messages, true); 
                        }
                        else if(unread_messages > 99) {
                            gtk_label_set_text(GTK_LABEL(new_chat->number_of_unread_messages), "+99");
                            gtk_widget_set_visible(new_chat->number_of_unread_messages, true);
                        }
                        get_num_of_msgs_with_user(call_data->ssl, new_chat->contact_id, new_chat->last_message_id, unread_messages + 15);
                    }
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
                            char *changed = cJSON_GetObjectItemCaseSensitive(message_data, "updated_at")->valuestring;
                            if (owner_id == GTK_data->user_id) {
                                chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(target_id));
                                add_message(message, time_to_send, true, (*changed) ? true : false, GTK_data->chat_manager, call_data->ssl, msg_id, chat, chat->contact_name);
                                if (i == 0) {
                                    chat->last_message_id = msg_id;
                                }
                                else if(i == all_mes_qty - 1) {
                                    if (local_time->tm_year > adjusted_time->tm_year || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon > adjusted_time->tm_mon) || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon == adjusted_time->tm_mon && local_time->tm_mday > adjusted_time->tm_mday)) {
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
                                add_message(message, time_to_send, false, (*changed) ? true : false, GTK_data->chat_manager, call_data->ssl, msg_id, chat, chat->contact_name);
                                if (i == 0) {
                                    chat->last_message_id = msg_id;
                                }
                                else if(i == all_mes_qty - 1) {
                                    if (local_time->tm_year > adjusted_time->tm_year || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon > adjusted_time->tm_mon) || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon == adjusted_time->tm_mon && local_time->tm_mday > adjusted_time->tm_mday)) {
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
                    if (counter == number_of_users) {
                        get_my_groups(call_data->ssl);
                    }
                    continue;
                }
                else if (command_code_json->valueint == 30 && stop_flag){
                    cJSON *groups = cJSON_GetObjectItemCaseSensitive(parsed_json, "groups");
                    groups_count = cJSON_GetObjectItemCaseSensitive(parsed_json, "groups_count")->valueint;
                    for (int i = 0; i < groups_count; i++) {
                        cJSON *group = cJSON_GetArrayItem(groups, i);
                        int chat_id = cJSON_GetObjectItemCaseSensitive(group, "chat_id")->valueint;
                        char *chat_name = cJSON_GetObjectItemCaseSensitive(group, "chat_name")->valuestring;
                        int unread_messages = cJSON_GetObjectItemCaseSensitive(group, "unread_mes_qty")->valueint;
                        
                        // Create a new scroll_data_t for the group
                        scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                        scroll_data->ssl = call_data->ssl;
                        chat_data_t *new_chat = create_chat_data(chat_name, chat_id, scroll_data);
                        create_group_in_sidebar(chat_id, chat_name, GTK_data, new_chat);
                        gtk_image_set_from_file(GTK_IMAGE(new_chat->avatar_circle), "src/chat_visual/images/group.svg");
                        
                        if (unread_messages > 0 && unread_messages < 99) {
                            gtk_label_set_text(GTK_LABEL(new_chat->number_of_unread_messages), mx_itoa(unread_messages));
                            gtk_widget_set_visible(new_chat->number_of_unread_messages, true); 
                        }
                        else if(unread_messages > 99) {
                            gtk_label_set_text(GTK_LABEL(new_chat->number_of_unread_messages), "+99");
                            gtk_widget_set_visible(new_chat->number_of_unread_messages, true);
                        }
                        get_num_of_msgs_from_group(call_data->ssl, chat_id, new_chat->last_message_id, unread_messages + 15);
                    }
                    stop_flag = false;
                    continue;
                }
                else if (command_code_json->valueint == 27 && group_counter < groups_count) {
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

                            int target_group_id = cJSON_GetObjectItemCaseSensitive(message_data, "target_group_id")->valueint;
                            char *message = cJSON_GetObjectItemCaseSensitive(message_data, "message")->valuestring;
                            int msg_id = cJSON_GetObjectItemCaseSensitive(message_data, "msg_id")->valueint;
                            char *changed = cJSON_GetObjectItemCaseSensitive(message_data, "updated_at")->valuestring;
                            chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(target_group_id));
                            int owner_id = cJSON_GetObjectItemCaseSensitive(message_data, "owner_id")->valueint;
                            char *nickname = cJSON_GetObjectItemCaseSensitive(message_data, "nickname")->valuestring;
                            add_message(message, time_to_send, owner_id == GTK_data->user_id, (*changed) ? true : false, GTK_data->group_manager, call_data->ssl, msg_id, chat, nickname);
                            if (i == 0) {
                                chat->last_message_id = msg_id;
                            }
                            else if(i == all_mes_qty - 1) {
                                if (local_time->tm_year > adjusted_time->tm_year || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon > adjusted_time->tm_mon) || 
                                    (local_time->tm_year == adjusted_time->tm_year && local_time->tm_mon == adjusted_time->tm_mon && local_time->tm_mday > adjusted_time->tm_mday)) {
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
                    group_counter++;
                    continue;
                }

                if(!stop_flag && counter == number_of_users){
                    switch (command_code_json->valueint)
                    {
                    cJSON *users;
                    int all_mes_qty;
                    chat_data_t *chat = NULL;
                    int sender_id;
                    int group_id;
                    case 31:
                        group_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "group_id")->valueint;
                        chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(group_id));
                        gtk_widget_set_visible(chat->number_of_unread_messages, false);
                        break;
                    case 27:
                        all_mes_qty = cJSON_GetObjectItemCaseSensitive(parsed_json, "all_mes_qty")->valueint;
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

                                int target_group_id = cJSON_GetObjectItemCaseSensitive(message_data, "target_group_id")->valueint;
                                char *message = cJSON_GetObjectItemCaseSensitive(message_data, "message")->valuestring;
                                int msg_id = cJSON_GetObjectItemCaseSensitive(message_data, "msg_id")->valueint;
                                char *changed = cJSON_GetObjectItemCaseSensitive(message_data, "updated_at")->valuestring;
                                int owner_id = cJSON_GetObjectItemCaseSensitive(message_data, "owner_id")->valueint;
                                chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(target_group_id));
                                char *nickname = cJSON_GetObjectItemCaseSensitive(message_data, "nickname")->valuestring;
                                add_message_to_top(message, time_to_send, owner_id == GTK_data->user_id, (*changed) ? true : false, GTK_data->group_manager, call_data->ssl, msg_id, chat, nickname);
                                if (i == 0) {
                                    chat->last_message_id = msg_id;
                                }
                            }
                        }
                        break;
                    case 26:
                        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(parsed_json, "success"))) {
                        // Extract the login string from the JSON
                        char *login = cJSON_GetObjectItemCaseSensitive(parsed_json, "login")->valuestring;

                        GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->deactivated_list));
                        while (child != NULL) {
                            // Get the text of the current child row
                            const char *current_login = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child))));
                            printf("\nlogin: %s, child: %s \n", login, current_login);

                            if (strcmp(current_login, login) == 0) {
                                // Remove the child row from the list box
                                gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->deactivated_list), child);

                                // Add the login to admin_login_list
                                GtkWidget *row = gtk_list_box_row_new();
                                GtkWidget *label = gtk_label_new(login);
                                gtk_widget_set_halign(label, GTK_ALIGN_START);
                                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->admin_login_list), row);

                                // Add the login to login_list
                                row = gtk_list_box_row_new();
                                label = gtk_label_new(login);
                                gtk_widget_set_halign(label, GTK_ALIGN_START);
                                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->login_list), row);

                                break; // Exit the loop since we've handled the match
                            }

                            // Move to the next child
                            GtkWidget *next_child = gtk_widget_get_next_sibling(child);
                            child = next_child;
                        }

                        // Free the cJSON object if it was dynamically allocated
                        cJSON_Delete(parsed_json);
                    }
                    break;
                    case 25:
                        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(parsed_json, "success"))) {
                        char *login = cJSON_GetObjectItemCaseSensitive(parsed_json, "login")->valuestring;

                        // Case 1: Remove from login_list
                        GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->login_list));
                        while (child != NULL) {
                            const char *current_login = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child))));
                            if (strcmp(current_login, login) == 0) {
                                gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->login_list), child);
                                break;
                            }
                            child = gtk_widget_get_next_sibling(child);
                        }

                        // Case 2: Remove from admin_login_list and add to deactivated_list
                        child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->admin_login_list));
                        while (child != NULL) {
                            const char *current_login = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child))));
                            if (strcmp(current_login, login) == 0) {
                                gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->admin_login_list), child);

                                GtkWidget *row = gtk_list_box_row_new();
                                GtkWidget *label = gtk_label_new(login);
                                gtk_widget_set_halign(label, GTK_ALIGN_START);
                                gtk_widget_set_margin_start(label, 10);
                                gtk_widget_set_margin_end(label, 10);
                                gtk_widget_set_margin_top(label, 5);
                                gtk_widget_set_margin_bottom(label, 5);
                                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->deactivated_list), row);
                                break;
                            }
                            child = gtk_widget_get_next_sibling(child);
                        }

                        // Free parsed JSON after processing
                        cJSON_Delete(parsed_json);
                    }
                        break;
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
                                char *changed = cJSON_GetObjectItemCaseSensitive(message_data, "updated_at")->valuestring;

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
                                    add_message_to_top(message, time_to_send, true, (*changed) ? true : false, GTK_data->chat_manager, call_data->ssl, msg_id, chat, "NULL");
                                }
                                else {
                                    chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(owner_id));
                                    add_message_to_top(message, time_to_send, false, (*changed) ? true : false, GTK_data->chat_manager, call_data->ssl, msg_id, chat, "NULL");
                                }
                            }
                        } 
                        break;

                    case 18:
                        sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                        chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                        gtk_widget_set_visible(chat->number_of_unread_messages, false);
                        break;
                    case 17:
                        users = cJSON_GetObjectItemCaseSensitive(parsed_json, "users");
                        number_of_users = cJSON_GetObjectItemCaseSensitive(parsed_json, "number_of_users")->valueint;

                        // Clear existing login lists
                        if (GTK_data->profile_data) {
                            // Clear login list
                            if (GTK_data->profile_data->login_list) {
                                GtkWidget *child;
                                while ((child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->login_list))) != NULL) {
                                    gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->login_list), child);
                                }
                            }

                            // Clear users list
                            if (GTK_data->profile_data->admin_login_list) {
                                GtkWidget *child;
                                while ((child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->admin_login_list))) != NULL) {
                                    gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->admin_login_list), child);
                                }
                            }

                            // Clear deactivated users list
                            if (GTK_data->profile_data->deactivated_list) {
                                GtkWidget *child;
                                while ((child = gtk_widget_get_first_child(GTK_WIDGET(GTK_data->profile_data->deactivated_list))) != NULL) {
                                    gtk_list_box_remove(GTK_LIST_BOX(GTK_data->profile_data->deactivated_list), child);
                                }
                            }

                            // Loop through users and populate the lists
                            cJSON *user;
                            cJSON_ArrayForEach(user, users) {
                                int user_id = cJSON_GetObjectItemCaseSensitive(user, "id")->valueint;
                                const char *login = cJSON_GetObjectItemCaseSensitive(user, "login")->valuestring;
                                bool is_admin = cJSON_GetObjectItemCaseSensitive(user, "admin")->valueint;
                                cJSON *active = cJSON_GetObjectItemCaseSensitive(user, "active");

                                // Skip "FirstAdmin"
                                if (user_id == 1 || g_strcmp0(login, "FirstAdmin") == 0) {
                                    continue;
                                }

                                if (cJSON_IsBool(active)) {
                                    if (cJSON_IsTrue(active)) {
                                        // Handle active users
                                        // Add non-admin users to the admin-login-list
                                        if (!is_admin && GTK_data->profile_data->admin_login_list) {
                                            GtkWidget *row = gtk_list_box_row_new();
                                            GtkWidget *label = gtk_label_new(login);
                                            gtk_widget_set_halign(label, GTK_ALIGN_START);
                                            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                            gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->admin_login_list), row);
                                        }

                                        // Add all users to the login list for deletion
                                        if (GTK_data->profile_data->login_list) {
                                            GtkWidget *row = gtk_list_box_row_new();
                                            GtkWidget *label = gtk_label_new(login);
                                            gtk_widget_set_halign(label, GTK_ALIGN_START);
                                            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                            gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->login_list), row);
                                        }
                                    } else {
                                        // Handle inactive users
                                        if (GTK_data->profile_data->deactivated_list) {
                                            GtkWidget *row = gtk_list_box_row_new();
                                            GtkWidget *label = gtk_label_new(login);
                                            gtk_widget_set_halign(label, GTK_ALIGN_START);
                                            gtk_widget_set_margin_start(label, 10);
                                            gtk_widget_set_margin_end(label, 10);
                                            gtk_widget_set_margin_top(label, 5);
                                            gtk_widget_set_margin_bottom(label, 5);
                                            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                            gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->deactivated_list), row);
                                        }
                                    }
                                }
                            }

                            // Ensure lists are visible
                            if (GTK_data->profile_data->login_list) {
                                gtk_widget_set_visible(GTK_WIDGET(GTK_data->profile_data->login_list), TRUE);
                            }
                            if (GTK_data->profile_data->admin_login_list) {
                                gtk_widget_set_visible(GTK_WIDGET(GTK_data->profile_data->admin_login_list), TRUE);
                            }
                            if (GTK_data->profile_data->deactivated_list) {
                                gtk_widget_set_visible(GTK_WIDGET(GTK_data->profile_data->deactivated_list), TRUE);
                            }
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

                                    int user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                                    char *login = cJSON_GetObjectItemCaseSensitive(parsed_json, "login")->valuestring;
                                    scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                                    scroll_data->ssl = call_data->ssl;
                                    chat_data_t *new_chat = create_chat_data(login, user_id, scroll_data);
                                    create_user_in_sidebar(user_id, login, false, GTK_data, new_chat);

                                    GtkWidget *row = gtk_list_box_row_new();
                                    GtkWidget *label = gtk_label_new(login);
                                    gtk_widget_set_halign(label, GTK_ALIGN_START);
                                    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                    gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->admin_login_list), row);

                                    row = gtk_list_box_row_new();
                                    label = gtk_label_new(login);
                                    gtk_widget_set_halign(label, GTK_ALIGN_START);
                                    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
                                    gtk_list_box_append(GTK_LIST_BOX(GTK_data->profile_data->login_list), row);
                                    
                                } else {
                                    // Error case
                                    cJSON *err_msg = cJSON_GetObjectItemCaseSensitive(json, "err_msg");
                                    char *error_text = (err_msg && cJSON_IsString(err_msg)) ? 
                                                        err_msg->valuestring : "Account creation failed";
                                    display_ui_message(GTK_data, error_text, false);
                                }
                            }
                        }

                        cJSON_Delete(json);
                        break;
                    }
                    case 2:
                        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(parsed_json, "success"))) {
                            // Print the parsed_json
                            // char *json_string = cJSON_Print(parsed_json); // Convert cJSON to string
                            // if (json_string) {
                            //     printf("\nparsed_json:\n %s\n\n", json_string); // Now it's a char *
                            //     free(json_string); // Free the allocated string to avoid memory leaks
                            // } else {
                            //     printf("Failed to print JSON\n");
                            // }
                            int chat_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_id")->valueint;
                            char *chat_name = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_name")->valuestring;

                            scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                            scroll_data->ssl = call_data->ssl;
                            chat_data_t *new_chat = create_chat_data(chat_name, chat_id, scroll_data);
                            create_group_in_sidebar(chat_id, chat_name, GTK_data, new_chat);
                            gtk_image_set_from_file(GTK_IMAGE(new_chat->avatar_circle), "src/chat_visual/images/group.svg");
                            // create_group_in_sidebar(chat_id, chat_name, GTK_data);
                        }
                        break;
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
                            
                            add_message(message, time_to_send, true, false, GTK_data->chat_manager, call_data->ssl, message_id, chat, "None");
                                
                            g_object_ref(chat->button);
                            gtk_box_remove(GTK_BOX(GTK_data->chat_manager->sidebar), chat->button);
                            gtk_box_prepend(GTK_BOX(GTK_data->chat_manager->sidebar), chat->button);
                            g_object_unref(chat->button);
                        }
                        break;

                    case 0:  
                        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(parsed_json, "success"))) {
                            int chat_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_id")->valueint;
                            int message_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                            // char *created_at = cJSON_GetObjectItemCaseSensitive(parsed_json, "time_reached_server")->valuestring;
                            char *message = cJSON_GetObjectItemCaseSensitive(parsed_json, "message")->valuestring;
                            chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(chat_id));

                            // strptime(created_at, "%Y-%m-%d %H:%M:%S", &message_time);
                            // time_t time_value = mktime(&message_time);
                            // time_value += time_zone; 
                            // adjusted_time = localtime(&time_value);
                            // strftime(time_to_send, sizeof(time_to_send), "%H:%M", adjusted_time);
                            add_message(message, time_to_send, true, false, GTK_data->group_manager, call_data->ssl, message_id, chat, "None");
                                
                            g_object_ref(chat->button);
                            gtk_box_remove(GTK_BOX(GTK_data->group_manager->sidebar), chat->button);
                            gtk_box_prepend(GTK_BOX(GTK_data->group_manager->sidebar), chat->button);
                            g_object_unref(chat->button);
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
                    if(!cJSON_GetObjectItemCaseSensitive(parsed_json, "message_type")->valueint) {
                        int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                        char *msg = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(parsed_json, "message"));
                        char time_str[6];
                        strftime(time_str, sizeof(time_str), "%H:%M", local_time);
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        add_message(msg, time_str, false, false, GTK_data->chat_manager, call_data->ssl, msg_id, chat, "None");
                        change_sidebar_chat_info(chat, msg, time_str);

                        if  (gtk_widget_get_visible(chat->number_of_unread_messages)) {
                            int unreaded = atoi(gtk_label_get_text(GTK_LABEL(chat->number_of_unread_messages))) + 1;

                            if (unreaded > 99) {
                                gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), "+99");
                            }
                            else {
                                gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), mx_itoa(unreaded));
                            }
                        }
                        else {
                            gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), "1");
                            gtk_widget_set_visible(chat->number_of_unread_messages, true); 
                        }
                        g_object_ref(chat->button);
                        gtk_box_remove(GTK_BOX(GTK_data->chat_manager->sidebar), chat->button);
                        gtk_box_prepend(GTK_BOX(GTK_data->chat_manager->sidebar), chat->button);
                        g_object_unref(chat->button);
                    }
                    else{
                        int chat_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_id")->valueint;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(chat_id));
                        char *msg = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(parsed_json, "message"));
                        char time_str[6];
                        strftime(time_str, sizeof(time_str), "%H:%M", local_time);
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        char *sender_nickname = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_nickname")->valuestring;
                        add_message(msg, time_str, false, false, GTK_data->group_manager, call_data->ssl, msg_id, chat, sender_nickname);
                        change_sidebar_chat_info(chat, msg, time_str);

                        if  (gtk_widget_get_visible(chat->number_of_unread_messages)) {
                            int unreaded = atoi(gtk_label_get_text(GTK_LABEL(chat->number_of_unread_messages))) + 1;

                            if (unreaded > 99) {
                                gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), "+99");
                            }
                            else {
                                gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), mx_itoa(unreaded));
                            }
                        }
                        else {
                            gtk_label_set_text(GTK_LABEL(chat->number_of_unread_messages), "1");
                            gtk_widget_set_visible(chat->number_of_unread_messages, true); 
                        }
                        g_object_ref(chat->button);
                        gtk_box_remove(GTK_BOX(GTK_data->group_manager->sidebar), chat->button);
                        gtk_box_prepend(GTK_BOX(GTK_data->group_manager->sidebar), chat->button);
                        g_object_unref(chat->button);
                    }
                    break;
                }
                case 52: { 
                    int chat_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_id")->valueint;
                    char *chat_name = cJSON_GetObjectItemCaseSensitive(parsed_json, "chat_name")->valuestring;
                    scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                    scroll_data->ssl = call_data->ssl;
                    chat_data_t *new_chat = create_chat_data(chat_name, chat_id, scroll_data);
                    create_group_in_sidebar(chat_id, chat_name, GTK_data, new_chat);
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
                case 55: {
                    int user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "id")->valueint;
                    char *nickname = cJSON_GetObjectItemCaseSensitive(parsed_json, "nickname")->valuestring;
                    scroll_data_t *scroll_data = g_new(scroll_data_t, 1);
                    scroll_data->ssl = call_data->ssl;
                    chat_data_t *new_chat = create_chat_data(nickname, user_id, scroll_data);
                    create_user_in_sidebar(user_id, nickname, false, GTK_data, new_chat);
                    break;
                }
                case 57: {
                    if(!cJSON_GetObjectItemCaseSensitive(parsed_json, "message_type")->valueint) {
                        int group_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "group_id")->valueint;
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        char *new_message = cJSON_GetObjectItemCaseSensitive(parsed_json, "new_message")->valuestring;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(group_id));
                        change_message_from_others(chat, msg_id, new_message);
                    }
                    else {
                        int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        char *new_message = cJSON_GetObjectItemCaseSensitive(parsed_json, "new_message")->valuestring;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(sender_id));
                        change_message_from_others(chat, msg_id, new_message);
                    }
                    break;
                }
                case 58: {
                    if(!cJSON_GetObjectItemCaseSensitive(parsed_json, "message_type")->valueint) {
                        int sender_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "sender_id")->valueint;
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->chat_manager->chats, GINT_TO_POINTER(sender_id));
                        delete_message_from_others(chat, msg_id);
                    }
                    else {
                        int group_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "group_id")->valueint;
                        int msg_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "message_id")->valueint;
                        chat_data_t *chat = g_hash_table_lookup(GTK_data->group_manager->chats, GINT_TO_POINTER(group_id));
                        delete_message_from_others(chat, msg_id);
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

            bool session_expired = false;

            SSL* new_ssl = try_to_reconnect(session_id, call_data->host, 
                                            call_data->port, &session_expired);

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
    free(local_time);
    free(session_id);
    pthread_detach(pthread_self());
    return NULL;
}

