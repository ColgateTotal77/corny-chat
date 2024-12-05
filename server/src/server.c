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
	client_t *client_data = call_data->client_data;

	if (client_data != NULL) {
        if (client_data->ssl != NULL) {
            // Critical resource access: CLIENT CONNECTION DATA. Start
	        pthread_mutex_lock(&client_data->mutex);
            SSL_shutdown(call_data->client_data->ssl); //Коректне завершення SSL-сесії
            SSL_free(call_data->client_data->ssl);
            client_data->ssl = NULL;
            pthread_mutex_unlock(&client_data->mutex);
	        // Critical resource access: CLIENT CONNECTION DATA. End

            printf("freeing ssl here\n");
            fflush(stdout);
        }
        
		if (client_data->user_data != NULL) {
            // Critical resource access: CLIENT USER DATA. Start
	        pthread_mutex_lock(&client_data->user_data->mutex);
            client_data->user_data->is_online = false;
            pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
	        // Critical resource access: CLIENT USER DATA. End
	    }
	}
	
	free(call_data);
}

void *handle_client(void *arg) {
	call_data_t *call_data = (call_data_t*)arg;
	general_data_t *general_data = call_data->general_data;

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
    bool is_active;

    if (call_data->client_data == NULL) {
        is_active = false;
    }
    else {
        // Critical resource access: CLIENT USER DATA. Start
	    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
	    is_active = call_data->client_data->user_data->is_active;
        pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
	    // Critical resource access: CLIENT USER DATA. End
    }

    while (!leave_flag && is_active && !general_data->server_stoped) {
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

        // Critical resource access: CLIENT USER DATA. Start
	    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
	    is_active = call_data->client_data->user_data->is_active;
        pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
	    // Critical resource access: CLIENT USER DATA. End
    }
    
    free_client_data(call_data);
    pthread_detach(pthread_self());

    return NULL;
}

void catch_ctrl_c_and_exit(int sig) {
	stop_server = true;
	printf("catched %d signal\n", sig);
}

void append_thread_to_array(pthread_t** arr, int* arr_size, pthread_t *thread_id) {
    *arr = (pthread_t*)realloc(*arr, (*arr_size + 1) * sizeof(pthread_t));
    (*arr)[*arr_size] = *thread_id;
    *arr_size = *arr_size + 1;
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

	general_data_t *general_data = setup_general_data(&stop_server, &online_count);
	pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t chats_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t login_to_id_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t session_id_to_id_mutex = PTHREAD_MUTEX_INITIALIZER;

    general_data->clients_mutex = &clients_mutex;
	general_data->chats_mutex = &chats_mutex;
	general_data->db_mutex = &db_mutex;
    general_data->login_to_id_mutex = &login_to_id_mutex;
    general_data->session_id_to_id_mutex = &session_id_to_id_mutex;
    general_data->server_stoped = false;
	
    pthread_t *threads_ids = malloc(0);
    int threads_count = 0;
    int connfd = 0;


    while (!stop_server) {
        connfd = accept(sock, NULL, NULL);

        if (connfd < 0) {
            fprintf(stderr, "accept() failed\n");
            continue;
        }

        if (general_data->server_stoped) {
            printf("Server is stopping. New connection rejected");
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

        pthread_t new_thread_id;

        pthread_create(&new_thread_id, NULL, &handle_client, (void*)call_data);

        append_thread_to_array(&threads_ids, &threads_count, &new_thread_id);

		/* Reduce CPU usage */
		sleep(1);
    }

	printf("Gracefully stoping\n");
	fflush(stdout);

    general_data->server_stoped = true;

    printf("Waiting for clients to disconnect\n");
	fflush(stdout);
      

    //for(int i = 0; i < threads_count; i++) {
    //    pthread_join(threads_ids[i], NULL);
    //}
    free(threads_ids);
    

    free_general_data(general_data);
    SSL_CTX_free(ctx);

    return EXIT_SUCCESS;
}


