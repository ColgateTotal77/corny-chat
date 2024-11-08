#include "cJSON.h"
#include "commands.h"
#include "client.h"

void see_my_contacts(int socket) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEE_MY_CONTACTS);

    send_and_delete_json(socket, &json);
}


