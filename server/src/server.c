#include "server.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "handlers.h"
#include "password.h"

bool stop_server;

static SSL_CTX *init_ssl_context(void) { 
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

static void free_client_data(call_data_t *call_data) {
	client_t *client_data = call_data->client_data;

	if (client_data != NULL) {
        if (client_data->ssl != NULL) {
            // Critical resource access: CLIENT CONNECTION DATA. Start
	        pthread_mutex_lock(&client_data->mutex);
            SSL_shutdown(call_data->client_data->ssl); //Коректне завершення SSL-сесії
            SSL_free(call_data->client_data->ssl);
            client_data->ssl = NULL;
            client_data->socket = -1;
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

static void *handle_client(void *arg) {
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

    int buffer_size = 1024;

	char buff_out[buffer_size];
	bzero(buff_out, buffer_size);
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
        int bytes_received = SSL_read(call_data->client_data->ssl, buff_out, buffer_size);
        if (bytes_received <= 0) {
            int err = SSL_get_error(call_data->client_data->ssl, bytes_received);
            if (err == SSL_ERROR_ZERO_RETURN) {
                break;
            }
            fprintf(stderr, "SSL_read failed with error: %d\n", err);
            //break;
        }
        else if (bytes_received > 1000) {
            printf("Ignoring invalidly long message\n");
        }
        handle_user_msg(bytes_received, &leave_flag, buff_out, call_data);
        bzero(buff_out, buffer_size);

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

static void catch_ctrl_c_and_exit(int sig) {
	stop_server = true;
	printf("catched %d signal\n", sig);
}

static void append_thread_to_array(pthread_t** arr, int* arr_size, pthread_t *thread_id) {
    *arr = (pthread_t*)realloc(*arr, (*arr_size + 1) * sizeof(pthread_t));
    (*arr)[*arr_size] = *thread_id;
    *arr_size = *arr_size + 1;
}

static void demonize_process(void) {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        exit(1);
    }
    if (pid > 0) {
        printf("daemon started with pid=%d\n", pid);
        printf("Use 'ps -aux | grep -E '%d'' to show process info\n", pid);
        printf("Use 'kill -SIGINT %d' to correctly stop process\n", pid);
        exit(0);
    }
    if (setsid() < 0) {
        exit(1);
    }

    //pid = fork();
//
    //if (pid < 0) {
    //    exit(1);
    //}
    //if (pid > 0) {
    //    printf("daemon started with pid=%d\n", pid);
    //    printf("Use 'ps -jx | grep 'PID\\|%d'' to show process info\n", pid);
    //    printf("Use 'kill -SIGINT %d' to correctly stop process\n", pid);
    //    exit(0);
    //}

    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
        close (x);
    }
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

    demonize_process();

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

    int *sockets = malloc(0);
    int sockets_count = 0;


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
        client_data->socket = connfd;
        
		call_data_t *call_data = (call_data_t*)malloc(sizeof(call_data_t));
        call_data->client_data = client_data;
		call_data->general_data = general_data;    

        pthread_t new_thread_id;

        pthread_create(&new_thread_id, NULL, &handle_client, (void*)call_data);

        append_thread_to_array(&threads_ids, &threads_count, &new_thread_id);
        append_to_intarr(&sockets, &sockets_count, connfd);

		/* Reduce CPU usage */
		sleep(1);
    }

	printf("Gracefully stoping\n");
	fflush(stdout);

    general_data->server_stoped = true;

    printf("Waiting for clients to disconnect\n");
	fflush(stdout);

    int cached_clients_count = 0;

    for (int i = 0; i < sockets_count; i++) {
        shutdown(sockets[i], SHUT_RDWR);
    }
    free(sockets);

    // Critical resource access: CLIENTS HASH TABLE. Start
	pthread_mutex_lock(general_data->clients_mutex);
	entry_t **clients_hash_slots = ht_dump(general_data->clients, &cached_clients_count);

	for (int i = 0; i < cached_clients_count; i++) {
		entry_t *entry = clients_hash_slots[i];
        client_t *client_data = entry->value;

		// Critical resource access: CLIENT CONNECTION DATA. Start
	    pthread_mutex_lock(&client_data->mutex);
		if (client_data->socket >= 0) {
			shutdown(client_data->socket, SHUT_RDWR);
	    }
	    pthread_mutex_unlock(&client_data->mutex);
	    // Critical resource access: CLIENT CONNECTION DATA. End
	}
	pthread_mutex_unlock(general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End
    free(clients_hash_slots);
      

    for(int i = 0; i < threads_count; i++) {
        pthread_join(threads_ids[i], NULL);
    }
    free(threads_ids);
    

    free_general_data(general_data);
    SSL_CTX_free(ctx);

    return EXIT_SUCCESS;
}


