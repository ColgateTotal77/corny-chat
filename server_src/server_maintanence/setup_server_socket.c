#include "server.h"

int setup_server_socket(int port) {
    int sock;
    struct sockaddr_in serv_addr;
    
    printf("Create socket() \n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock < 0) {
        fprintf(stderr, "socket() failed: %d\n", errno);
        return EXIT_FAILURE;
    }
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    signal(SIGPIPE, SIG_IGN);
    int option = 1;

    //if (setsockopt(sock, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option, sizeof(option)) < 0){
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,(char*)&option, sizeof(option)) < 0){
        fprintf(stderr, "ERROR: setsockopt failed");
        return EXIT_FAILURE;
	}
    
    printf("Binding to socket... \n");
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "bind() failed: %d\n", errno);
        return EXIT_FAILURE;
    }
    
    printf("Listening to socket... \n");

    int max_length_of_pending_connections = 10;
    if (listen(sock, max_length_of_pending_connections) < 0) {
        fprintf(stderr, "ERROR: setsockopt failed");
        return EXIT_FAILURE;
    }

	return sock;
}


