#include "server.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "handlers.h"
#include "password.h"

bool stop_server;

SSL_CTX *init_ssl_context(void) { 
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    const SSL_METHOD *method = TLS_server_method();

	SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1); 
    }

    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;  
}

void free_client_data(call_data_t *call_data) {
	general_data_t *general_data = call_data->general_data;
	client_t *client_data = call_data->client_data;

	if (client_data != NULL) {
        SSL_shutdown(call_data->client_data->ssl); //Коректне завершення SSL-сесії
        SSL_free(call_data->client_data->ssl);
        client_data->ssl = NULL;
	    //close(client_data->sockfd);
	    //client_data->sockfd = -1;
		if (client_data->user_data != NULL) {
            client_data->user_data->is_online = false;
	    }
	}
	
    *(general_data->online_count) = *(general_data->online_count) - 1;
	free(call_data);
}

void *handle_client(void *arg) {
	call_data_t *call_data = (call_data_t*)arg;
	general_data_t *general_data = call_data->general_data;
	
	*(general_data->online_count) = *(general_data->online_count) + 1;

	char str_json_login_password[BUF_SIZE];
	bzero(str_json_login_password, BUF_SIZE);

    int bytes = SSL_read(call_data->client_data->ssl, str_json_login_password, BUF_SIZE);
    printf("%s\n", str_json_login_password);
    fflush(stdout);
    if (bytes <= 0) {
        int err = SSL_get_error(call_data->client_data->ssl, bytes);
        fprintf(stderr, "SSL_read failed with error: %d\n", err);
        ERR_print_errors_fp(stderr);
        free_client_data(call_data);
        pthread_detach(pthread_self());
        return NULL;
    }
    

    int leave_flag = 0;
	leave_flag = handle_login(str_json_login_password, call_data);

	char buff_out[BUF_SIZE];
	bzero(buff_out, BUF_SIZE);

    while (!leave_flag) {
        int bytes_received = SSL_read(call_data->client_data->ssl, buff_out, BUF_SIZE);
        if (bytes_received <= 0) {
            int err = SSL_get_error(call_data->client_data->ssl, bytes_received);
            if (err == SSL_ERROR_ZERO_RETURN) {
                break;
            }
            fprintf(stderr, "SSL_read failed with error: %d\n", err);
            //break;
        }
        handle_user_msg(bytes_received, &leave_flag, buff_out, call_data);
        bzero(buff_out, BUF_SIZE);
    }
    
    free_client_data(call_data);
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

    if (access("admin.txt", F_OK)) {
        sqlite3 *db; //connection variable
        int rc = sqlite3_open("db/uchat_db.db", &db); //open DB connection
        
        if (rc) { //check is connect successful
            sqlite3_close(db);
        }
        unsigned char *new_password_hash = create_admin();
        update_password_hash(db, 1, new_password_hash);
        free(new_password_hash);
        sqlite3_close(db);
    }

	SSL_CTX *ctx = init_ssl_context();

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
    int connfd = 0;


    while (!stop_server) {
        connfd = accept(sock, NULL, NULL);

        if (connfd < 0) {
            fprintf(stderr, "accept() failed\n");
            continue;
        }

        if ((online_count + 1) == MAX_CLIENTS) {
            printf("Max clients reached. New connection rejected");
            close(connfd);
            continue;
		}

        SSL *ssl = SSL_new(ctx);
        if (!ssl) {
            perror("Unable to create SSL structure");
            close(connfd);
            continue;
        }

        SSL_set_fd(ssl, connfd); //Прив'язка об'єкта SSL до сокету для TLS-з'єднання

        if (SSL_accept(ssl) <= 0) { //Встановлення захисного з'єднання
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(connfd);
            continue;
        }

        client_t *client_data = (client_t*)malloc(sizeof(client_t));
		//client_data->sockfd = connfd;
		client_data->user_data = NULL;
        client_data->ssl = ssl;
        
		call_data_t *call_data = (call_data_t*)malloc(sizeof(call_data_t));
        call_data->client_data = client_data;
		call_data->general_data = general_data;    
        call_data->client_data->ssl = ssl;

        pthread_create(&new_thread_id, NULL, &handle_client, (void*)call_data);

		/* Reduce CPU usage */
		sleep(1);
    }

	printf("Gracefully stoping\n");
	fflush(stdout);

    free_general_data(general_data);
    SSL_CTX_free(ctx);  

    return EXIT_SUCCESS;
}


