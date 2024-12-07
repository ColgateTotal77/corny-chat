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
#include <netdb.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#include "cJSON.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define BUF_SIZE 256

typedef struct{
	int sockfd;
	SSL *ssl;
	bool *stop_flag;
	char name[32];
	char *host;
	int port;
} call_data_t;


int setup_client_socket(char *hostname, int port);
cJSON* get_name_password(void);
void str_del_newline(char* arr, int length);
char *read_tls_socket(SSL *ssl);
void* recv_msg_handler(void* arg);
void* send_msg_handler(void* arg);
int recieve_next_response(SSL *ssl, char **buffer);
SSL *try_to_reconnect(char *session_id, char *host, int port, bool *session_expired);
void* recv_login_msg_handler(void* arg);
SSL *setup_new_connection(char *host, int port);
void* reconnect_handler(void* arg);
