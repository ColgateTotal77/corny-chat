#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_my_groups(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_MY_GROUPS); 

    send_and_delete_json(ssl, &json);
}
