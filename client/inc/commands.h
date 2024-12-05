#pragma once

#include <openssl/ssl.h>
#include "command_codes.h"
#include "stdbool.h"



void send_to_chat(SSL *ssl, int chat_id, char *message);
void send_to_user(SSL *ssl, int reciever_id, char *message);
void see_all_chats(SSL *ssl);
void see_all_users(SSL *ssl);
void create_chat(SSL *ssl, char *new_chat_name);
void send_exit_command(SSL *ssl);
void add_contact_to_chat(SSL *ssl, int contact_id, int chat_id);
void join_chat(SSL *ssl, int chat_id);
void see_my_contacts(SSL *ssl);
void add_contact(SSL *ssl, int new_contact_id);
void get_my_contacts(SSL *ssl);
void get_all_talks(SSL *ssl);
void update_my_nickname(SSL *ssl, char *new_nickname);
void change_password(SSL *ssl, char *old_password, char *new_password);
void create_new_user(SSL *ssl, char *login, char *password, bool is_admin);
void get_all_clients_userslist(SSL *ssl);
void mark_chat_msgs_as_readed(SSL *ssl, int sender_id);
void get_my_clients_userslist(SSL *ssl);
void delete_user_from_group(SSL *ssl, int user_id, int chat_id);
void add_many_users_to_group(SSL *ssl, int* users_id, int users_count, int chat_id);
void delete_group(SSL *ssl, int group_id);
void admin_change_password(SSL *ssl, char *user_login, char *new_password);
void get_num_of_msgs_with_user(SSL *ssl, int user2, int before, int quantity);
void deactivate_user(SSL *ssl, char *user_login);
void activate_user(SSL *ssl, char *login);
void get_num_of_msgs_from_group(SSL *ssl, int group_id, int before, int quantity);
void update_message(SSL *ssl, int message_id, char *new_message);
void delete_message(SSL *ssl, int message_id);
void get_my_groups(SSL *ssl);
void mark_group_msgs_as_readed(SSL *ssl, int group_id);
void get_chat_users(SSL *ssl, int chat_id);


void send_and_delete_json(SSL *ssl, cJSON **json);

