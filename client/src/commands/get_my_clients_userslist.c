#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_my_clients_userslist(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_MY_CLIENTS_USERSLIST); 

    send_and_delete_json(ssl, &json);
}
