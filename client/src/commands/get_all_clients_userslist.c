#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_all_clients_userslist(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_ALL_CLIENTS_USERSLIST); 

    send_and_delete_json(ssl, &json);
}
