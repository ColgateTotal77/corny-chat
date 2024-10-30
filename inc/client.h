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
#define BUF_SIZE 256


typedef struct{
	int sockfd;
	bool *stop_flag;
	char name[32];
} call_data_t;


int setup_client_socket(char *hostname, int port);


