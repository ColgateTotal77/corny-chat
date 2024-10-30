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

#include "hashTable.h"

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
    char name[32];
    int user_id;
} user_t;

typedef struct {
	struct sockaddr_in address;
	int sockfd;
	user_t* user_data;
} client_t;

typedef struct {
    client_t *client_data;
    ht_t *clients;
    ht_t *chats;
    pthread_mutex_t *clients_mutex;
    pthread_mutex_t *chats_mutex;
    int *clients_count;
    int *chat_uid;
} call_data_t;


int setup_server_socket(int port);
void handle_user_msg(int bytes_received, int *leave_flag, char *client_msg, call_data_t *call_data);



void send_message_to_id(call_data_t *call_data, char *message, int user_id);
void send_message_to_user(call_data_t *call_data, char *message);
void send_message_to_another_ids(call_data_t *call_data, char *s);
void send_message_to_chat(call_data_t *call_data, char *message, int chat_id);
void send_user_exit_msg(call_data_t *call_data);


