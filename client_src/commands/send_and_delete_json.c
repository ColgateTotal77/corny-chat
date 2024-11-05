#include "client.h"


void send_and_delete_json(int socket, cJSON **json) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    send(socket, str_json, strlen(str_json), 0);

    free(str_json);
    cJSON_Delete(*json);
}

