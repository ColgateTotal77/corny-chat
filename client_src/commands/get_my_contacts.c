#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_my_contacts(int socket) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_MY_CONTACTS); 

    send_and_delete_json(socket, &json);
}

