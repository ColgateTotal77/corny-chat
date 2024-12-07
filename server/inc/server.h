#pragma once
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "hashTable.h"
#include "cJSON.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define BUF_SIZE 256
#define MAX_CLIENTS 100
#define MAX_CHATS 20


typedef struct {
    int* users_id;
    int users_count;
    char name[50];
    char createdAt[20];
    int chat_id;
    int owner_id;
    pthread_mutex_t mutex;
} chat_t;

typedef struct {
    int user_id;
    char login[32];
    char nickname[50];
    int* contacts_id;
    int contacts_count;
    int* groups_id;
    int groups_count;
    bool is_online;
    bool is_admin;
    bool is_active;
    pthread_mutex_t mutex;
} user_t;

typedef struct {
	char time_created_session_id[20];
    SSL *ssl;
	user_t* user_data;
    pthread_mutex_t mutex;
} client_t;

typedef struct {
    ht_str_t *login_to_id;
    ht_str_t *session_id_to_id;
    ht_t *clients;
    ht_t *chats;
    pthread_mutex_t *clients_mutex;
    pthread_mutex_t *chats_mutex;
    pthread_mutex_t *db_mutex;
    pthread_mutex_t *login_to_id_mutex;
    pthread_mutex_t *session_id_to_id_mutex;
    int *online_count;
    sqlite3* db;
    bool server_stoped;
} general_data_t;

typedef struct {
    client_t *client_data;
    general_data_t *general_data;
   // SSL *ssl;
} call_data_t;



enum LoginValidationResult {
    INVALID_INPUT,
    VALID_LOGIN_VIA_SESSION_ID,
    VALID_LOGIN,
};


// server_maintanence
int setup_server_socket(int port);
general_data_t *setup_general_data(bool *stop_server, int *online_count);
void free_general_data(general_data_t *general_data);
void add_offline_user_to_server_cache(sqlite3 *db, ht_t *clients, ht_str_t *login_to_id,
                                      int user_id, char *login, char *nickname, 
                                      bool is_admin, bool is_active);

void handle_user_msg(int bytes_received, int *leave_flag,
                     char *client_msg, call_data_t *call_data);
enum LoginValidationResult find_and_validate_user(call_data_t *call_data, 
                                               cJSON *json_name_password);
int handle_login(char *str_json_name_password, call_data_t *call_data);

// utility_functions
user_t* init_user_data(sqlite3 *db, int id, char *login, char *nickname, 
                       bool is_admin, bool is_active, bool is_online);

int create_new_user_and_return_id(call_data_t *call_data, char *login,
                                  unsigned char* password_hash, bool is_admin);
char* get_string_time(void);
char *add_length_to_string(char *str);
bool user_has_such_contact(user_t *user_data, int contact_id);
bool group_has_such_user(chat_t *chat_data, int user_id);
bool user_exists(call_data_t *call_data, int user_id);
bool group_exists(call_data_t *call_data, int group_id);
void append_to_group_users(chat_t *chat_data, int user_id);
void append_to_users_groups(user_t *user_data, int chat_id);
void update_group_users_and_user_groups(chat_t *chat_data, client_t *client_data);
bool is_user_group_owner(call_data_t *call_data, int group_id);
void append_to_users_contacts(user_t *user_data, int contact_id);
void remove_from_group_users(chat_t *chat_data, int user_id);
void remove_from_users_groups(user_t *user_data, int chat_id);
bool get_group_data_and_lock_group_mutex(call_data_t *call_data, int group_id, 
                                         chat_t **chat_data);

// send_msg_functions
void send_message_to_id(call_data_t *call_data, char *message, int user_id);
void send_message_to_user(call_data_t *call_data, char *message);
void send_message_to_another_ids(call_data_t *call_data, char *s);
void send_message_to_chat(call_data_t *call_data, char *message, int chat_id);
void send_to_user_and_delete_json(call_data_t *call_data, cJSON **json);
void send_to_id_and_delete_json(call_data_t *call_data, cJSON **json, int id_to_send);
void send_to_chat_and_delete_json(call_data_t *call_data, cJSON **json, int chat_id);
void send_to_another_ids_and_delete_json(call_data_t *call_data, cJSON **json);
void send_user_returned_msg(call_data_t *call_data);
void send_user_exit_msg(call_data_t *call_data);
void send_to_client_and_delete_json(cJSON **json, client_t* client_data);
void send_to_group_and_delete_json(call_data_t *call_data, cJSON **json, chat_t *chat);
void send_json_to_client(cJSON **json, client_t* client_data);
void send_to_user_and_delete_json_no_mutexes(call_data_t *call_data, cJSON **json);
void send_to_group_and_delete_json_no_ht_mutex(call_data_t *call_data, 
                                               cJSON **json, chat_t *chat);

cJSON *create_response_json(int command_code, bool success_status, char *error_msg);

