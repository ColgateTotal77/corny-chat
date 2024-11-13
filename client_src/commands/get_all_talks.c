#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_all_talks(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_ALL_TALKS); 

    send_and_delete_json(ssl, &json);
}
