#include "server.h"
#include "../libmx/inc/libmx.h"


bool stop_server;

void *handle_client(void *arg) {
    char buff_out[BUF_SIZE];
	char name[32];
	int leave_flag = 0;
	
	call_data_t *call_data = (call_data_t*)arg;
	client_t *client_data = call_data->client_data;
	ht_t *clients = call_data->clients;
	*(call_data->clients_count) = *(call_data->clients_count) + 1;

    if (recv(client_data->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1) {
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	}
    else {
		strcpy(client_data->user_data->name, name);
		sprintf(buff_out, "%s has joined\n", client_data->user_data->name);
		printf("%s", buff_out);
		send_message_to_another_ids(call_data, buff_out);
	}

    bzero(buff_out, BUF_SIZE);
	

	while (!leave_flag) {
		int bytes_received = recv(client_data->sockfd, buff_out, BUF_SIZE, 0);
		handle_user_msg(bytes_received, &leave_flag, buff_out, call_data);
		bzero(buff_out, BUF_SIZE);
	}

    close(client_data->sockfd);
    ht_del(clients, client_data->user_data->user_id);
	free(client_data->user_data->contacts_id);
    free(client_data->user_data);
    free(client_data);
    *(call_data->clients_count) = *(call_data->clients_count) - 1;
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
    //psa.sa_handler = catch_ctrl_c_and_exit;
    sigaction(SIGINT, &psa, NULL);
	stop_server = false;
    
    printf("Server Started\n");

    
    pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t chats_mutex = PTHREAD_MUTEX_INITIALIZER;
    int clients_count = 0;
    int uid = 0;
	int chat_uid = 0;

	ht_t *clients = ht_create();
    ht_t *chats = ht_create();
    

    pthread_t new_thread_id;
    struct sockaddr_in client_addr;
    socklen_t client_data_len;
    int connfd = 0;

    chat_t *odd_chat = (chat_t *)malloc(sizeof(chat_t));
    odd_chat->users_id = (int*)malloc(0);
    odd_chat->users_count = 0;
    odd_chat->chat_id = chat_uid++;
    strcpy(odd_chat->name, "odd_chat");

	ht_set(chats, odd_chat->chat_id, (void*)odd_chat);

    while (!stop_server) {
        
        client_data_len = sizeof(client_addr);

        connfd = accept(sock, (struct sockaddr*)&client_addr, &client_data_len);

        if (connfd < 0) {
            fprintf(stderr, "accept() failed\n");
            continue;
        }

        if ((clients_count + 1) == MAX_CLIENTS) {
            printf("Max clients reached. New connection rejected");
            close(connfd);
            continue;
		}

        user_t *user_data = (user_t*)malloc(sizeof(user_t));
        user_data->user_id = uid++;
        user_data->contacts_count = 0;
		user_data->contacts_id = malloc(0);

        client_t *client_data = (client_t*)malloc(sizeof(client_t));
		client_data->address = client_addr;
		client_data->sockfd = connfd;
		client_data->user_data = user_data;

		call_data_t *call_data = (call_data_t*)malloc(sizeof(call_data_t));
		call_data->client_data = client_data;
		call_data->clients = clients;
		call_data->chats = chats;
		call_data->clients_mutex = &clients_mutex;
		call_data->chats_mutex = &chats_mutex;
		call_data->clients_count = &clients_count;
		call_data->chat_uid = &chat_uid;
		

        if (user_data->user_id % 2 != 0) {
            append_to_intarr(&odd_chat->users_id, &odd_chat->users_count, user_data->user_id);
        }

		ht_set(clients, user_data->user_id, (void*)client_data);

        pthread_create(&new_thread_id, NULL, &handle_client, (void*)call_data);

		/* Reduce CPU usage */
		sleep(1);
    }

	printf("Gracefully stoping\n");
	fflush(stdout);

    int chats_count = 0;
	entry_t **chat_hash_slots = ht_dump(chats, &chats_count);

	for (int i = 0; i < chats_count; i++) {
		entry_t *entry = chat_hash_slots[i];
        chat_t *chat_data = entry->value;
		free(chat_data->users_id);
		free(chat_data);
		free(entry);
	}
	free(chat_hash_slots);
	free(chats->entries);
	free(chats);

	free(clients->entries);
	free(clients);

    
    return EXIT_SUCCESS;
}


