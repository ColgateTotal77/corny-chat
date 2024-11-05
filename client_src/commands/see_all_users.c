#include "cJSON.h"
#include "commands.h"
#include "client.h"

void see_all_users(int socket) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEE_ALL_USERS);

    send_and_delete_json(socket, &json);
}

