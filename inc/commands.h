#pragma once


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
#define GET_MY_CONTACTS 10



void send_to_chat(int socket, int chat_id, char *message);
void send_to_user(int socket, int reciever_id, char *message);
void see_all_chats(int socket);
void see_all_users(int socket);
void create_chat(int socket, char *new_chat_name);
void send_exit_command(int socket);
void add_contact_to_chat(int socket, int contact_id, int chat_id);
void join_chat(int socket, int chat_id);
void see_my_contacts(int socket);
void add_contact(int socket, int new_contact_id);
void get_my_contacts(int socket);



void send_and_delete_json(int socket, cJSON **json);

