#include "client.h"

int setup_client_socket(char *hostname, int port) {
    printf ("Creating socket...\n");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 
    if (sock < 0) {
        printf("socket() failed: %d", errno);
        return EXIT_FAILURE;
    }
    
    printf ("Looking for the host...\n");
    struct hostent *server = gethostbyname(hostname);
    
    if (server == NULL) {
        printf("Host not found\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr;

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, (server->h_addr_list)[0], server->h_length);
    serv_addr.sin_port = htons(port);
    printf ("Connecting to server...\n");
 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("connect() failed: %d", errno);
        return EXIT_FAILURE;
    }

    printf("Client started\n");

    return sock;
}
