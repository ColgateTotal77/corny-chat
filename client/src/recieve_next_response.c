#include "client.h"
#include <ctype.h>

/**
 * @brief Get whole response from server even when it length exceeds buffer (1024) size
 * @warning this function allocate memory do not forget to free buffer
 *
 * @param ssl working ssl connection
 * @param buffer pointer to "char* text = NULL;" variable. 
 *               It will be used to return the result string
 * @return number of bytes recived, either whole result length or 0, -1 in case of an errors
 */
int recieve_next_response(SSL *ssl, char **buffer) {
    char message[1024];
    bzero(message, 1024);
    int bytes_received = SSL_read(ssl, message, 1024);

    if (bytes_received == 0) {
        return 0;
    }
    else if (bytes_received < 0) {
        return -1;
    }

    char *cur_char = message;
    int bytes_to_await = 0;
    int number_length = 0;

    while (*cur_char != '\0' && isdigit(*cur_char)) {
        number_length++;
        bytes_to_await = bytes_to_await * 10 + (*cur_char - '0');
        cur_char++;
    }

    char *whole_message_buffer = (char*)calloc(bytes_to_await + 1, sizeof(char));

    strncpy(whole_message_buffer, message + number_length, bytes_received - number_length);
    int whole_message_length = strlen(whole_message_buffer);

    bytes_to_await -= bytes_received - number_length;

    while (bytes_to_await > 0) {
        bzero(message, 1024);
        bytes_received = SSL_read(ssl, message, 1024);

        if (bytes_received == 0) {
            free(whole_message_buffer);
            return 0;
        }
        else if (bytes_received < 0) {
            free(whole_message_buffer);
            return -1;
        }

        strncpy(whole_message_buffer + whole_message_length, message, bytes_received);
        whole_message_length += bytes_received;
        bytes_to_await -= bytes_received;
    }

    *buffer = whole_message_buffer;
    return whole_message_length;
}
