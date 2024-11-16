#include "client.h"


void send_and_delete_json(SSL *ssl, cJSON **json) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    SSL_write(ssl, str_json, strlen(str_json));
    
    free(str_json);
    cJSON_Delete(*json);
}

