#include "client.h"

bool stop_flag;

void str_overwrite_stdout() {
    printf("%s", "> ");
    fflush(stdout);
}

//void* handle_user_input(void* arg) {
//    call_data_t* call_data = (call_data_t*)arg;
//    char message[BUF_SIZE];
//
//    while (1) {
//
//    }
//
//    *(call_data->stop_flag) = true;
//    
//    return NULL;
//}

void* send_msg_handler(void* arg) {
    call_data_t* call_data = (call_data_t*)arg;
    char message[BUF_SIZE];
    char buffer[BUF_SIZE + 32];

    while (!*(call_data->stop_flag)) {
        str_overwrite_stdout();
        fgets(message, BUF_SIZE, stdin);
        str_del_newline(message, BUF_SIZE);

        if (strcmp(message, "/exit") == 0) { break; }

        sprintf(buffer, "%s: %s\n", call_data->name, message);
        send(call_data->sockfd, buffer, strlen(buffer), 0);

        bzero(message, BUF_SIZE);
        bzero(buffer, BUF_SIZE + 32);
    }

    *(call_data->stop_flag) = true;
    shutdown(call_data->sockfd, SHUT_RDWR);
    
    pthread_detach(pthread_self());

    return NULL;
}

void* recv_msg_handler(void* arg) {
    call_data_t* call_data = (call_data_t*)arg;
    char message[BUF_SIZE];

    while (!*(call_data->stop_flag)) {
        int bytes_received = recv(call_data->sockfd, message, BUF_SIZE, 0);
        

        if (bytes_received > 0) {
            printf("%s", message);
            str_overwrite_stdout();
        }
        else if (bytes_received == 0) {
        	break;
        }
        else {
			printf("ERROR: -1\n");
            fflush(stdout);
		}

		memset(message, 0, sizeof(message));
    }

    pthread_detach(pthread_self());
    
    return NULL;
}

void catch_ctrl_c_and_exit(int sig) {
    (void)sig;
    stop_flag = true;
    printf("Press ENTER to end process\n");
    fflush(stdout);
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <hostname> <port_number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    cJSON *json_name_and_password = get_name_password();
    
    int port = atoi(argv[2]);

    int sock = setup_client_socket(argv[1], port);

    struct sigaction psa = {.sa_handler = catch_ctrl_c_and_exit};
    sigaction(SIGINT, &psa, NULL);
    stop_flag = false;

    char *str_json_name_password = cJSON_Print(json_name_and_password);
    cJSON_Minify(str_json_name_password);

    send(sock, str_json_name_password, strlen(str_json_name_password), 0);

    free(str_json_name_password);

    cJSON *name_json = cJSON_GetObjectItemCaseSensitive(json_name_and_password, "name");
    
    call_data_t *call_data = (call_data_t *)malloc(sizeof(call_data_t));
    call_data->sockfd = sock;
    call_data->stop_flag = &stop_flag;
    strcpy(call_data->name, name_json->valuestring);

    cJSON_Delete(json_name_and_password);
    
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, &send_msg_handler, (void*)call_data) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
	}

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, (void*)call_data) != 0) {
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

    while (1) {
		if (stop_flag) {
            pthread_join(send_msg_thread, NULL);
			printf("\nBye\n");
			break;
        }
        sleep(1);
    }

    free(call_data);
    close(sock);
    
    return EXIT_SUCCESS;
}

