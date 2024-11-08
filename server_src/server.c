#include "server.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "handlers.h"
#include "password.h"

bool stop_server;


void *handle_client(void *arg) {
	call_data_t *call_data = (call_data_t*)arg;
	general_data_t *general_data = call_data->general_data;
	
	*(general_data->online_count) = *(general_data->online_count) + 1;

	char str_json_name_password[BUF_SIZE];
	bzero(str_json_name_password, BUF_SIZE);
	int leave_flag = 0;

	recv(call_data->client_data->sockfd, str_json_name_password, BUF_SIZE, 0);
	
    printf("%s\n", str_json_name_password);
	fflush(stdout);

	leave_flag = handle_login(str_json_name_password, call_data);

    client_t *client_data = call_data->client_data;
	char buff_out[BUF_SIZE];
	bzero(buff_out, BUF_SIZE);

	while (!leave_flag) {
		int bytes_received = recv(client_data->sockfd, buff_out, BUF_SIZE, 0);
		handle_user_msg(bytes_received, &leave_flag, buff_out, call_data);
		bzero(buff_out, BUF_SIZE);
	}
	

    close(client_data->sockfd);
	client_data->sockfd = -1;
	client_data->address = NULL;
	if (client_data->user_data != NULL) {
        client_data->user_data->is_online = false;
	}
    *(general_data->online_count) = *(general_data->online_count) - 1;
	free(call_data);
    pthread_detach(pthread_self());

	return NULL;
}

void catch_ctrl_c_and_exit(int sig) {
	stop_server = true;
	printf("catched %d signal\n", sig);
}

int main(int argc, char * argv[]) {
	if (argc != 2) {
        fprintf(stderr, "usage: %s <port_number>\n", argv[0]);
        return EXIT_FAILURE;
    }

	int port = atoi(argv[1]);
	int sock = setup_server_socket(port);

    struct sigaction psa = {.sa_handler = catch_ctrl_c_and_exit};
    sigaction(SIGINT, &psa, NULL);
	stop_server = false;
    
    printf("Server Started\n");

    int online_count = 0;
	int chat_uid = 0;

	general_data_t *general_data = setup_general_data(&stop_server, &online_count, &chat_uid);
	pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t chats_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
	//pthread_mutex_t general_data_mutex = PTHREAD_MUTEX_INITIALIZER;
    general_data->clients_mutex = &clients_mutex;
	general_data->chats_mutex = &chats_mutex;
	general_data->db_mutex = &db_mutex;
	//general_data->general_data_mutex = general_data_mutex;
	
    pthread_t new_thread_id;
    struct sockaddr_in client_addr;
    socklen_t client_data_len;
    int connfd = 0;


    while (!stop_server) {
        client_data_len = sizeof(client_addr);

        connfd = accept(sock, (struct sockaddr*)&client_addr, &client_data_len);

        if (connfd < 0) {
            fprintf(stderr, "accept() failed\n");
            continue;
        }

        if ((online_count + 1) == MAX_CLIENTS) {
            printf("Max clients reached. New connection rejected");
            close(connfd);
            continue;
		}


        client_t *client_data = (client_t*)malloc(sizeof(client_t));
		client_data->address = &client_addr;
		client_data->sockfd = connfd;
		client_data->user_data = NULL;

		call_data_t *call_data = (call_data_t*)malloc(sizeof(call_data_t));
		call_data->client_data = client_data;
		call_data->general_data = general_data;
		

        //if (user_data->user_id % 2 != 0) {
        //    append_to_intarr(&odd_chat->users_id, &odd_chat->users_count, user_data->user_id);
        //}
        
        pthread_create(&new_thread_id, NULL, &handle_client, (void*)call_data);

		/* Reduce CPU usage */
		sleep(1);
    }

	printf("Gracefully stoping\n");
	fflush(stdout);

    free_general_data(general_data);
    
    return EXIT_SUCCESS;
}


