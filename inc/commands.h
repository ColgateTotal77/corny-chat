#pragma once

#include <openssl/ssl.h>
#define SEND_TO_CHAT 0
#define SEND_TO_USER 1
#define CREATE_CHAT 2
#define SEE_ALL_CHATS 3
#define SEE_ALL_USERS 4
#define ADD_CONTACT 5
#define SEE_MY_CONTACTS 6
#define JOIN_CHAT 7
#define ADD_CONTACT_TO_CHAT 8
#define EXIT 9



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



void send_and_delete_json(SSL *ssl, cJSON **json);

