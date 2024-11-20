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
    char name[32];
    int chat_id;
} chat_t;

typedef struct {
    int* contacts_id;
    int contacts_count;
    int* groups_id;
    int groups_count;
    char login[32];
    char nickname[50];
    int user_id;
    bool is_online;
} user_t;

typedef struct {
	char time_created_session_id[20];
	//int sockfd;
    SSL *ssl;
	user_t* user_data;
} client_t;

typedef struct {
    ht_str_t *login_to_id;
    ht_str_t *session_id_to_id;
    ht_t *clients;
    ht_t *chats;
    pthread_mutex_t *clients_mutex;
    pthread_mutex_t *chats_mutex;
    pthread_mutex_t *db_mutex;
    int *online_count;
    int *chat_uid;
    sqlite3* db;
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
general_data_t *setup_general_data(bool *stop_server, int *online_count, int *chat_uid);
void free_general_data(general_data_t *general_data);
void add_offline_user_to_server_cache(sqlite3 *db, ht_t *clients, ht_str_t *login_to_id,
                                      int user_id, char *login, char *nickname);

void handle_user_msg(int bytes_received, int *leave_flag, char *client_msg, call_data_t *call_data);
enum LoginValidationResult find_and_validate_user(call_data_t *call_data, 
                                               cJSON *json_name_password);
int handle_login(char *str_json_name_password, call_data_t *call_data);

// utility_functions
user_t* init_user_data(sqlite3 *db, int id, char *name, char *nickname, bool is_online);
void send_to_user_and_delete_json(call_data_t *call_data, cJSON **json);
void send_to_id_and_delete_json(call_data_t *call_data, cJSON **json, int id_to_send);
void send_to_chat_and_delete_json(call_data_t *call_data, cJSON **json, int chat_id);
int create_new_user_and_return_id(call_data_t *call_data, char *login, unsigned char* password_hash);
char* get_string_time(void);
char *add_length_to_string(char *str);

// send_msg_functions
void send_message_to_id(call_data_t *call_data, char *message, int user_id);
void send_message_to_user(call_data_t *call_data, char *message);
void send_message_to_another_ids(call_data_t *call_data, char *s);
void send_message_to_chat(call_data_t *call_data, char *message, int chat_id);
void send_user_exit_msg(call_data_t *call_data);

cJSON *create_response_json(int command_code, bool success_status, char *error_msg);

